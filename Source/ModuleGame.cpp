#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleGame.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"
#include "Colliders.h"
#include <vector>
#include <algorithm>
#include <memory>
#include <fstream>

ModuleGame::ModuleGame(Application* app, bool start_enabled) : Module(app, start_enabled) {}
ModuleGame::~ModuleGame() {}

bool ModuleGame::Start()
{
    SetTargetFPS(60);

    tAMR23 = LoadTexture("Assets/Textures/AMR23.png");
    tR25 = LoadTexture("Assets/Textures/R25.png");
    tGP2Engine = LoadTexture("Assets/Textures/GP2Engine.png");
    tMp4 = LoadTexture("Assets/Textures/Mc4.png");
    tMp22 = LoadTexture("Assets/Textures/Mc22.png");
    tPinkMerc = LoadTexture("Assets/Textures/PinkMerc.png");
    tW11 = LoadTexture("Assets/Textures/W11.png");
    tRB21 = LoadTexture("Assets/Textures/RedBull(Tututuru).png");
    track = LoadTexture("Assets/Textures/Track.png");

    leftArrow = LoadTexture("Assets/Textures/Car_Selection1.png");
    rightArrow = LoadTexture("Assets/Textures/Car_Selection2.png");

    App->renderer->DrawInsideCamera = [this]() { if (gameState == GameState::Gameplay) DrawGameplay(); };
    App->renderer->DrawAfterBegin = [this]() { DrawUI(); };

    carList.push_back({ tAMR23, "AMR23", "33"});
    carList.push_back({ tGP2Engine, "GP2", "Where is palmer?"});
    carList.push_back({ tW11, "W11" , "Hammer time"});
    carList.push_back({ tPinkMerc, "PinkMerc", "Rosa"});
    carList.push_back({ tR25, "R25", "WE ARE THE CHAMPIONS"});
    carList.push_back({ tMp4, "Mc4", "RIP"});
    carList.push_back({ tMp22, "Mc22", "MAFIA"});
    carList.push_back({ tRB21, "RB21", "EL QUINTO?? Ya no ;("});

    gameState = GameState::InitialMenu;

    return true;
}

update_status ModuleGame::Update()
{
    // === HACER CLASES CON LOS DIFERENTES ESTADOS DEL JUEGO ===
    if (IsKeyPressed(KEY_F5)) player.lap = 99999;
    float dt = GetFrameTime();

    coroutineManager.Update(dt);

    switch (gameState)
    {
    case GameState::InitialMenu: InitialMenu(dt); break;
    case GameState::Gameplay:    Gameplay(dt); break;
    case GameState::EndGame:     EndGameMenu(dt); break;
    }

    return UPDATE_CONTINUE;
}

// === INITIAL MENU === 
void ModuleGame::InitialMenu(float dt)
{
    InitialMenuStart();

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Vector2 mouse = GetMousePosition();
        b2Vec2 pMousePos = { PIXELS_TO_METERS(mouse.x), PIXELS_TO_METERS(mouse.y) };

        for (auto fixture : App->physics->GetFixtures())
        {
            if (fixture->TestPoint(pMousePos))
            {
                PhysBody* ui = (PhysBody*)fixture->GetBody()->GetUserData().pointer;
                if (!ui || ui->id.empty()) continue;

                if (ui->id == "CAR_LEFT") {
                    currentCarIndex = (currentCarIndex - 1 + carList.size()) % carList.size();
                    menuCar->carTexture = carList[currentCarIndex].texture;
                }
                else if (ui->id == "CAR_RIGHT") {
                    currentCarIndex = (currentCarIndex + 1) % carList.size();
                    menuCar->carTexture = carList[currentCarIndex].texture;
                }
                else if (ui->id == "CAR_DISPLAY") {
                    player.texture = menuCar->carTexture;
                    playerIdSelected = carList[currentCarIndex].id;
                    ui->selectable = false;
                    gameState = GameState::Gameplay;
                }
                else if (ui->id == "UI") {
                    if (ui->isLeft && player.totalLaps > 3) player.totalLaps--;
                    else if (!ui->isLeft && player.totalLaps < 72) player.totalLaps++;
                }
            }
        }
    }
}

// === GAMEPLAY ===
void ModuleGame::Gameplay(float dt)
{
    GameplayStart();
    CarsUpdate(dt);
    GameManager(dt);
}

void ModuleGame::EndGameMenu(float dt)
{
    timeToNextState += dt;
    if (timeToNextState >= 1.0f)
    {
        gameState = GameState::InitialMenu;
    }
}

void ModuleGame::InitialMenuStart()
{
    // OPTIMIZAR / HACER CLASE / MEJORAR DE ALGUNA FORMA
    if (initialMenuStart) return;
    gamePlayStart = false;
    currentCarIndex = 0;

    menuCar = App->physics->CreateRectangle(
        SCREEN_WIDTH / 2,
        SCREEN_HEIGHT / 2,
        29 * 2, 73 * 2,
        0.0f,
        true,
        0,
        ColliderType::UI,
        STATIC
    );

    menuCar->id = "CAR_DISPLAY";
    menuCar->carTexture = carList[currentCarIndex].texture;

    leftArrowCar = App->physics->CreateRectangle(
        SCREEN_WIDTH / 2 - 150,
        SCREEN_HEIGHT / 2,
        16, 28,
        0.0f, true, this, ColliderType::UI, STATIC
    );
    leftArrowCar->id = "CAR_LEFT";

    rightArrowCar = App->physics->CreateRectangle(
        SCREEN_WIDTH / 2 + 150,
        SCREEN_HEIGHT / 2,
        16, 28,
        0.0f, true, this, ColliderType::UI, STATIC
    );
    rightArrowCar->id = "CAR_RIGHT";

    leftArrowLap = App->physics->CreateRectangle(SCREEN_WIDTH / 2 - 60, SCREEN_HEIGHT - 100, 13, 21, 0.0f, true, this, ColliderType::UI, STATIC);
    leftArrowLap->id = "UI";
    righttArrowLap = App->physics->CreateRectangle(SCREEN_WIDTH / 2 + 60, SCREEN_HEIGHT - 100, 13, 21, 0.0f, true, this, ColliderType::UI, STATIC);
    righttArrowLap->id = "UI";
    righttArrowLap->isLeft = false;

    uiPhys.push_back(menuCar);
    uiPhys.push_back(leftArrowCar);
    uiPhys.push_back(rightArrowCar);
    uiPhys.push_back(leftArrowLap);
    uiPhys.push_back(righttArrowLap);

    initialMenuStart = true;
}

void ModuleGame::GameplayStart()
{
    if (gamePlayStart) return;

    // Destroy ui
    for (auto ui : uiPhys)
        Destroy(ui);
    uiPhys.clear();

    // Creation of the cars after car is selected
    player.Start({ SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 });
    player.canMove = false;
    player.pbody->id = playerIdSelected;

    lightsOut = false;
    allCars.push_back(&player);

    for (int i = 0; i < 7; ++i)
    {
        AICar* ai = new AICar();
        ai->Start({ (float)SCREEN_WIDTH / 2 + i * 100, SCREEN_HEIGHT / 2 });
        aiCars.push_back(ai);
        allCars.push_back(ai);
    }

    // Sets the player for the camera
    App->renderer->SetPlayer(&player);

    AssignAICars();

    // Creation of the checkered flags
    checkeredFlag = App->physics->CreateRectangle(600, 500, 20, 280, 0.0f, true, this, ColliderType::CHECKEREDFLAG, STATIC);

    if (LoadChainFromFile("Assets/ColliderPoints/Track_External_Points.txt", externalTrackPoints))
        Colliders* circuit = new Colliders(0, 0, externalTrackPoints.data(), externalTrackPoints.size(), ColliderType::WALL, this);

    if (LoadChainFromFile("Assets/ColliderPoints/InternalTrackPointsS1.txt", internalTrackPointsS1))
        Colliders* circuit = new Colliders(0, 0, internalTrackPointsS1.data(), internalTrackPointsS1.size(), ColliderType::WALL, this);

    if (LoadChainFromFile("Assets/ColliderPoints/InternalTrackPointsS2.txt", internalTrackPointsS2))
        Colliders* circuit = new Colliders(0, 0, internalTrackPointsS2.data(), internalTrackPointsS2.size(), ColliderType::WALL, this);

    if (LoadChainFromFile("Assets/ColliderPoints/SensorTrackPointsAbove.txt", sensorTrackPointsAbove))
        Colliders* circuit = new Colliders(0, 0, sensorTrackPointsAbove.data(), sensorTrackPointsAbove.size(), ColliderType::WALL, this);

    if (LoadChainFromFile("Assets/ColliderPoints/SensorTrackPointsBelow.txt", sensorTrackPointsBelow))
        Colliders* circuit = new Colliders(0, 0, sensorTrackPointsBelow.data(), sensorTrackPointsBelow.size(), ColliderType::WALL, this);

    checkpoints.push_back((std::make_unique<Checkpoint>(1249, 1220, 20, 270, 1, 45, this)));
    checkpoints.push_back((std::make_unique<Checkpoint>(2051, 1589, 20, 270, 2, 0, this)));
    checkpoints.push_back((std::make_unique<Checkpoint>(2566, 1185, 20, 260, 3, 120, this)));
    checkpoints.push_back((std::make_unique<Checkpoint>(2694, 1494, 20, 250, 4, 100, this)));
    checkpoints.push_back((std::make_unique<Checkpoint>(2758, 2489, 20, 250, 5, 90, this)));
    checkpoints.push_back((std::make_unique<Checkpoint>(3736, 2217, 20, 250, 6, 110, this)));
    checkpoints.push_back((std::make_unique<Checkpoint>(4629, 2516, 20, 290, 7, 90, this)));
    checkpoints.push_back((std::make_unique<Checkpoint>(6066, 4180, 20, 270, 8, 30, this)));
    checkpoints.push_back((std::make_unique<Checkpoint>(5307, 2155, 20, 280, 9, 60, this)));
    checkpoints.push_back((std::make_unique<Checkpoint>(3869, 1599, 20, 290, 10, -50, this)));
    checkpoints.push_back((std::make_unique<Checkpoint>(2388, 2089, 20, 270, 11, 20, this)));
    checkpoints.push_back((std::make_unique<Checkpoint>(442, 1076, 20, 270, 12, 55, this)));

    App->physics->CreateCircle(1000, 1220, 50, true, this, ColliderType::DIRT, STATIC);

    std::sort(allCars.begin(), allCars.end(),
        [](Car* a, Car* b)
        {
            if (a->lap != b->lap)
                return a->lap > b->lap;

            if (a->checkpoint != b->checkpoint)
                return a->checkpoint > b->checkpoint;

            return a->distanceToNextCheckpoint < b->distanceToNextCheckpoint;
        });

    for (int i = 0; i < allCars.size(); i++)
        allCars[i]->racePosition = i + 1;

    timeToNextState = 0.0f;

    gamePlayStart = true;
    initialMenuStart = false;
}

void ModuleGame::TrafficLight()
{
    if (lightsOut) return;
    //player.canMove = true; // QUITAR
    lightTimer += GetFrameTime();

    if (lightTimer > 1.0f)
        DrawCircle(SCREEN_WIDTH / 2, 100, 20, RED);
    if (lightTimer > 2.0f)
        DrawCircle(SCREEN_WIDTH / 2, 150, 20, RED);
    if (lightTimer > 3.0f)
        DrawCircle(SCREEN_WIDTH / 2, 200, 20, RED);

    if (lightTimer > 4.0f)
    {
        lightsOut = true;
        //player.canMove = true;
        lightTimer = 0.0f;
    }
}

// === CARS UPDATE ===
void ModuleGame::CarsUpdate(float dt)
{
    player.Update(dt);
    for (auto ai : aiCars) ai->Update(dt);
}

void ModuleGame::GameManager(float dt)
{
    if (player.lap <= player.totalLaps)
    {
        if (player.lap > 0)
        {
            player.currentLapTime += GetFrameTime();
            showLap = player.lap;

            UpdatePosition();
        }
    }

    else
    {
        timeToNextState += dt;
        player.canMove = false;
        // Activar modo IA una vez acabada la carrera?

        if (timeToNextState >= 1.0f)
        {
            for (auto ai : aiCars)
                ai->Destroy();

            aiCars.clear();
            player.Destroy();

            checkpoints.clear();
            App->physics->DestroyBody(checkeredFlag);

            allCars.clear();

            timeToNextState = 0;

            gameState = GameState::EndGame;
        }
    }
}

void ModuleGame::UpdatePosition()
{
    std::sort(allCars.begin(), allCars.end(),
        [](Car* a, Car* b)
        {
            if (a->lap != b->lap)
                return a->lap > b->lap;

            if (a->checkpoint != b->checkpoint)
                return a->checkpoint > b->checkpoint;

            return a->distanceToNextCheckpoint < b->distanceToNextCheckpoint;
        });

    for (int i = 0; i < allCars.size(); i++)
        allCars[i]->racePosition = i + 1;
}

// === DRAWING FUNCTIONS ===
void ModuleGame::DrawGameplay()
{
    // Draw the track
    DrawTextureEx(track, { 0,0 }, 0, 4.0f, WHITE);

    // Draw the cars
    CarsDraw();
}

void ModuleGame::DrawUI()
{
    if (gameState == GameState::InitialMenu)
    {
        // Draw the initial menu
        DrawInitialMenu();
        return;
    }
    else if (gameState == GameState::Gameplay)
    {
        // Draw the traffic light
        TrafficLight();

        int startX = 20;
        int startY = 180;
        int lineSpacing = 22;

        for (size_t i = 0; i < allCars.size(); i++)
        {
            Car* car = allCars[i];
            DrawText(TextFormat("P%d  %s",
                (int)i + 1,
                car->pbody->id.c_str()),
                startX,
                startY + i * lineSpacing,
                20,
                BLACK);
        }

        // Draw gameplay UI
        DrawText(TextFormat("Lap Time: %.2f", player.currentLapTime), 20, 50, 20, BLACK);
        DrawText(TextFormat("Previous Lap Time: %.2f", player.previousLapTime), 20, 70, 20, BLACK);
        DrawText(TextFormat("Fastest Lap Time: %.2f", player.fastestLapTime), 20, 120, 20, BLACK);

        // Draw the current number of laps & the total
        std::string lapText = TextFormat("Lap: %d/%d", showLap, player.totalLaps);
        int lapWidth = MeasureText(lapText.c_str(), 20);
        DrawText(lapText.c_str(), SCREEN_WIDTH - lapWidth - 20, 50, 20, BLACK);

        return;
    }
    else
    {
        // Draw the total laps & the fastest lap
        std::string lapsText = TextFormat("%d Laps", player.totalLaps);
        int lapsWidth = MeasureText(lapsText.c_str(), 50);
        DrawText(lapsText.c_str(), SCREEN_WIDTH / 2 - lapsWidth / 2, 150, 50, BLACK);

        std::string fastestLapText = TextFormat("Fastest Lap: %.2f", player.fastestLapTime);
        int fastestWidth = MeasureText(fastestLapText.c_str(), 50);
        DrawText(fastestLapText.c_str(), SCREEN_WIDTH / 2 - fastestWidth / 2, 200, 50, BLACK);

        return;
    }
}

void ModuleGame::DrawInitialMenu()
{
    CarInfo& carShown = carList[currentCarIndex];

    // Draw the car to show
    DrawTextureEx(menuCar->carTexture, { SCREEN_WIDTH / 2 - 30, SCREEN_HEIGHT / 2 + 73 }, -90, 2, WHITE);

    DrawTextureEx(leftArrow, { SCREEN_WIDTH / 2 - 158, SCREEN_HEIGHT / 2 - 14 }, 0, 2, WHITE);
    DrawTextureEx(rightArrow, { SCREEN_WIDTH / 2 + 141, SCREEN_HEIGHT / 2 - 14 }, 0, 2, WHITE);

    // Draw the description
    int descWidth = MeasureText(carShown.description.c_str(), 30);
    DrawText(carShown.description.c_str(), SCREEN_WIDTH / 2 - descWidth / 2, SCREEN_HEIGHT / 2 - 110, 30, BLACK);

    // Draw the number of total laps
    std::string lapsText = TextFormat("%d Laps", player.totalLaps);
    int lapsWidth = MeasureText(lapsText.c_str(), 20);
    DrawText(lapsText.c_str(), SCREEN_WIDTH / 2 - lapsWidth / 2, SCREEN_HEIGHT - 105, 20, BLACK);

    DrawTextureEx(leftArrow, { SCREEN_WIDTH / 2  - 67, SCREEN_HEIGHT - 110 }, 0, 1.5f, WHITE);
    DrawTextureEx(rightArrow, { SCREEN_WIDTH / 2 + 53, SCREEN_HEIGHT - 110 }, 0, 1.5f, WHITE);
}

void ModuleGame::AssignAICars()
{
    // Hacer clase?? Tengo que mejorar la estructura del código
    std::vector<int> available;

    for (int i = 0; i < carList.size(); i++)
    {
        if (i != currentCarIndex)
            available.push_back(i);
    }

    std::random_shuffle(available.begin(), available.end());

    int indexAI = 0;

    for (auto ai : aiCars)
    {
        if (indexAI >= available.size()) break;

        int idx = available[indexAI++];

        ai->texture = carList[idx].texture;
        ai->pbody->id = carList[idx].id;
    }
}

void ModuleGame::CarsDraw()
{
    // Draw the cars
    player.Draw();
    for (auto ai : aiCars) ai->Draw();
}

bool ModuleGame::LoadChainFromFile(const char* path, std::vector<int>& outPoints)
{
    std::ifstream file(path);

    if (!file.is_open())
        return false;

    int x, y;

    while (file >> x >> y) {
        outPoints.push_back(x);
        outPoints.push_back(y);
    }

    return true;
}

bool ModuleGame::CleanUp()
{
    // Clean up
    player.CleanUp();

    for (auto ai : aiCars)
    {
        ai->CleanUp();
        delete ai;
    }
    aiCars.clear();
    allCars.clear();

    pAMR23      = NULL;
    pR25        = NULL;
    pGP2Engine  = NULL;
    pMp4        = NULL;
    pMp22       = NULL;
    pPinkMerc   = NULL;
    pW11        = NULL;
    pRB21       = NULL;

    checkeredFlag = NULL;

    menuCar         = NULL;
    leftArrowLap    = NULL;
    righttArrowLap  = NULL;
    leftArrowCar    = NULL;
    rightArrowCar   = NULL;

    UnloadTexture(tAMR23);
    UnloadTexture(tGP2Engine);
    UnloadTexture(tW11);
    UnloadTexture(tMp22);
    UnloadTexture(tMp4);
    UnloadTexture(tPinkMerc);
    UnloadTexture(tR25);
    UnloadTexture(tRB21);

    return true;
}

void ModuleGame::OnCollision(PhysBody* physA, PhysBody* physB)
{
    switch (physB->ctype)
    {
    case ColliderType::PLAYER:
        if (physA->ctype == ColliderType::CHECKEREDFLAG)
        {
            LOG("Checkered flag detected");
            if (player.checkpoint == checkpoints.size() || player.lap == 0)
            {
                player.lap++;
                player.checkpoint = 0;

                player.previousLapTime = player.currentLapTime;
                if (player.fastestLapTime > player.currentLapTime || player.lap == 2 ) 
                    player.fastestLapTime = player.currentLapTime;
                player.currentLapTime = 0.0f;
            }
        }

        else if (physA->ctype == ColliderType::CHECKPOINT)
        {
            if (player.checkpoint + 1 == physA->n)
                player.checkpoint++;
        }

        else if (physA->ctype == ColliderType::DIRT)
        {
            player.inDirt = true;
        }
        break;

    case ColliderType::AICAR:
        if (physA->ctype == ColliderType::CHECKEREDFLAG)
        {
            LOG("Checkered flag detected");

            auto car = dynamic_cast<AICar*>(physB->listener);
            if (car->checkpoint == checkpoints.size() || car->lap == 0)
            {
                car->lap++;
                car->checkpoint = 0;

                car->previousLapTime = car->currentLapTime;
                if (car->fastestLapTime > car->currentLapTime || car->lap == 2)
                    car->fastestLapTime = car->currentLapTime;
                car->currentLapTime = 0.0f;
            }
        }

        else if (physA->ctype == ColliderType::CHECKPOINT)
        {
            auto car = dynamic_cast<AICar*>(physB->listener);
            if (car != NULL)
                if (car->checkpoint + 1 == physA->n)
                    car->checkpoint++;
        }

        else if (physA->ctype == ColliderType::DIRT)
        {
            auto car = dynamic_cast<AICar*>(physB->listener);
            if (car != NULL) car->inDirt = true;
        }
        break;

    default:
        break;
    }
}

void ModuleGame::EndCollision(PhysBody* physA, PhysBody* physB)
{
    switch (physB->ctype)
    {
    case ColliderType::PLAYER:
        if (physA->ctype == ColliderType::DIRT)
        {
            player.inDirt = false;
        }

    case ColliderType::AICAR:
        if (physA->ctype == ColliderType::DIRT)
        {
            auto car = dynamic_cast<AICar*>(physB->listener);
            if (car != NULL) car->inDirt = false;
        }
        break;

    default:
        break;
    }
}