#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleGame.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"
#include <vector>
#include <algorithm>
#include <memory>
#include <fstream>

ModuleGame::ModuleGame(Application* app, bool start_enabled) : Module(app, start_enabled) {}
ModuleGame::~ModuleGame() {}

bool ModuleGame::Start()
{
    SetTargetFPS(60);

    tAMR23 = LoadTexture("Assets/Textures/Cars/AMR23.png");
    tR25 = LoadTexture("Assets/Textures/Cars/R25.png");
    tGP2Engine = LoadTexture("Assets/Textures/Cars/GP2Engine.png");
    tMp4 = LoadTexture("Assets/Textures/Cars/Mc4.png");
    tMp22 = LoadTexture("Assets/Textures/Cars/Mc22.png");
    tPinkMerc = LoadTexture("Assets/Textures/Cars/PinkMerc.png");
    tW11 = LoadTexture("Assets/Textures/Cars/W11.png");
    tRB21 = LoadTexture("Assets/Textures/Cars/RedBull(Tututuru).png");
    track = LoadTexture("Assets/Textures/Track.png");

    amr23Stats = LoadTexture("Assets/Textures/Stats/amr23_stats.png");
    r25Stats = LoadTexture("Assets/Textures/Stats/r25_stats.png");
    w11Stats = LoadTexture("Assets/Textures/Stats/w11_stats.png");
    gp2Stats = LoadTexture("Assets/Textures/Stats/gp2_stats.png");
    pinkMercStats = LoadTexture("Assets/Textures/Stats/pinkMerc_stats.png");
    mc4Stats = LoadTexture("Assets/Textures/Stats/mc4_stats.png");
    mc22Stats = LoadTexture("Assets/Textures/Stats/mc22_stats.png");
    rb21Stats = LoadTexture("Assets/Textures/Stats/rb21_stats.png");

    initialMenuScreen = LoadTexture("Assets/Textures/UI/Main_menu.png");
    endScreen = LoadTexture("Assets/Textures/UI/Endgame.png");

    leftArrow = LoadTexture("Assets/Textures/UI/Car_Selection1.png");
    rightArrow = LoadTexture("Assets/Textures/UI/Car_Selection2.png");

    carList.push_back({ tAMR23,     "AMR23",    amr23Stats });
    carList.push_back({ tGP2Engine, "MCL33",    gp2Stats });
    carList.push_back({ tW11,       "W11" ,     w11Stats });
    carList.push_back({ tPinkMerc,  "RP20",     pinkMercStats });
    carList.push_back({ tR25,       "R25",      r25Stats });
    carList.push_back({ tMp4,       "MP4-4",    mc4Stats });
    carList.push_back({ tMp22,      "MP4-22",   mc22Stats });
    carList.push_back({ tRB21,      "RB21",     rb21Stats });

    App->renderer->DrawInsideCamera = [this]() { if (gameState == GameState::Gameplay) DrawGameplay(); };
    App->renderer->DrawAfterBegin = [this]() { DrawUI(); };

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
    if (timeToNextState >= 5.0f)
    {
        allCars.clear();
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
        328,
        SCREEN_HEIGHT / 2,
        29 * 4, 73 * 4,
        0.0f,
        true,
        0,
        ColliderType::UI,
        STATIC
    );

    menuCar->id = "CAR_DISPLAY";
    menuCar->carTexture = carList[currentCarIndex].texture;

    leftArrowCar = App->physics->CreateRectangle(
        194,
        SCREEN_HEIGHT / 2,
        16, 28,
        0.0f, true, this, ColliderType::UI, STATIC
    );
    leftArrowCar->id = "CAR_LEFT";

    rightArrowCar = App->physics->CreateRectangle(
        463,
        SCREEN_HEIGHT / 2,
        16, 28,
        0.0f, true, this, ColliderType::UI, STATIC
    );
    rightArrowCar->id = "CAR_RIGHT";

    leftArrowLap = App->physics->CreateRectangle(SCREEN_WIDTH / 2 - 78, SCREEN_HEIGHT - 100, 13, 21, 0.0f, true, this, ColliderType::UI, STATIC);
    leftArrowLap->id = "UI";
    righttArrowLap = App->physics->CreateRectangle(SCREEN_WIDTH / 2 + 78, SCREEN_HEIGHT - 100, 13, 21, 0.0f, true, this, ColliderType::UI, STATIC);
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
    player.Start({ 5500, 2340});
    player.canMove = false;
    player.pbody->id = playerIdSelected;

    lightsOut = false;
    allCars.push_back(&player);

    float startAngle = -150.0f;
    float startAngleRad = startAngle * DEG2RAD;

    float dirX = cosf(startAngleRad);
    float dirY = sinf(startAngleRad);

    float spacing = 100.0f;

    for (int i = 0; i < 7; ++i)
    {
        AICar* ai = new AICar();

        Vector2 spawnPos = {
            5550.0f + dirX * spacing * i,
            2320.0f + dirY * spacing * i
        };

        ai->Start(spawnPos);
        aiCars.push_back(ai);
        allCars.push_back(ai);
    }


    // Sets the player for the camera
    App->renderer->SetPlayer(&player);

    AssignAICars();

    // Creation of the checkered flags
    checkeredFlag = App->physics->CreateRectangle(5670, 2670, 20, 280, 60.0f, true, this, ColliderType::CHECKEREDFLAG, STATIC);

    CreateColliders();

    sensorAbove = App->physics->CreateRectangle(3050, 2245, 20, 400, 0.0f, true, this, ColliderType::SENSOR, STATIC);
    sensorBelow = App->physics->CreateRectangle(2720, 1900, 350, 20, 0.0f, true, this, ColliderType::SENSOR, STATIC);

    checkpoints.push_back((std::make_unique<Checkpoint>(6362, 3632, 20, 280, 1, 60, this)));
    checkpoints.push_back((std::make_unique<Checkpoint>(5992, 4146, 20, 400, 2, 50, this)));
    checkpoints.push_back((std::make_unique<Checkpoint>(5735, 3805, 20, 400, 3, 90, this)));
    checkpoints.push_back((std::make_unique<Checkpoint>(5535, 3550, 20, 400, 4, 0, this)));
    checkpoints.push_back((std::make_unique<Checkpoint>(5285, 3385, 20, 400, 5, 80, this)));
    checkpoints.push_back((std::make_unique<Checkpoint>(5144, 3015, 20, 400, 6, 50, this)));
    checkpoints.push_back((std::make_unique<Checkpoint>(4842, 2926, 20, 400, 7, 35, this)));
    checkpoints.push_back((std::make_unique<Checkpoint>(4614, 2596, 20, 400, 8, 90, this)));
    checkpoints.push_back((std::make_unique<Checkpoint>(4744, 2214, 20, 400, 9, 90, this)));
    checkpoints.push_back((std::make_unique<Checkpoint>(4380, 1895, 20, 400, 10, 0, this)));
    checkpoints.push_back((std::make_unique<Checkpoint>(3974, 1982, 20, 400, 11, -10, this)));


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
    player.canMove = true; // QUITAR
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
        player.canMove = true;
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

            for (auto phys : trackPhys)
                App->physics->DestroyBody(phys);

            trackPhys.clear();
            checkpoints.clear();
            App->physics->DestroyBody(checkeredFlag);

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
        // Draw the background
        DrawTextureEx(endScreen, { 0,0 }, 0, 1, WHITE);

        // Draw the total laps & the fastest lap
        std::string lapsText = TextFormat("%d Laps", player.totalLaps);
        int lapsWidth = MeasureText(lapsText.c_str(), 50);
        DrawText(lapsText.c_str(), 200 - lapsWidth / 2, 70, 20, BLACK);

        int nameX = 120;
        int startX = 80;
        int startY = 130;
        int fastLapX = 210;
        int lineSpacing = 50;

        for (size_t i = 0; i < allCars.size(); i++)
        {
            Car* car = allCars[i];
            const char* placeText = (i == 0) ? " " : TextFormat("%d", (int)i + 1);
            int y = startY + i * lineSpacing;

            // Cars position
            DrawText(placeText, startX, y, 20, BLACK);

            // Name
            DrawText(car->pbody->id.c_str(), nameX, y, 20, BLACK);

            // Fast lap
            DrawText(TextFormat("Fast lap: %.2f", car->fastestLapTime),
                fastLapX, y, 20, BLACK);
        }

        return;
    }
}

void ModuleGame::DrawInitialMenu()
{
    CarInfo& carShown = carList[currentCarIndex];

    // Draw background
    DrawTexture(initialMenuScreen, 0, 0, WHITE);

    // Draw the car to show
    DrawTextureEx(menuCar->carTexture, { 270, SCREEN_HEIGHT / 2 + 146 }, - 90, 4, WHITE);

    // Draw the description
    DrawTexture(carShown.imageToShow, 0, 0, WHITE);

    DrawTextureEx(leftArrow, { 186, SCREEN_HEIGHT / 2 - 14 }, 0, 2, WHITE);
    DrawTextureEx(rightArrow, { 454, SCREEN_HEIGHT / 2 - 14 }, 0, 2, WHITE);

    // Draw the number of total laps
    std::string lapsText = TextFormat("%d Laps", player.totalLaps);
    int lapsWidth = MeasureText(lapsText.c_str(), 20);
    DrawText(lapsText.c_str(), SCREEN_WIDTH / 2 - lapsWidth / 2 - 17, SCREEN_HEIGHT - 115, 30, BLACK);

    DrawTextureEx(leftArrow, { SCREEN_WIDTH / 2  - 85, SCREEN_HEIGHT - 110 }, 0, 1.5f, WHITE);
    DrawTextureEx(rightArrow, { SCREEN_WIDTH / 2 + 71, SCREEN_HEIGHT - 110 }, 0, 1.5f, WHITE);
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

void ModuleGame::CreateColliders()
{
    if (LoadChainFromFile("Assets/ColliderPoints/Track_External_Points.txt", externalTrackPoints))
        trackExt = new Colliders(
            0, 0,
            externalTrackPoints.data(),
            externalTrackPoints.size(),
            ColliderType::WALL, this,
            trackPhys);

    if (LoadChainFromFile("Assets/ColliderPoints/InternalTrackPointsS1.txt", internalTrackPointsS1))
        trackIntS1 = new Colliders(
            0, 0,
            internalTrackPointsS1.data(),
            internalTrackPointsS1.size(),
            ColliderType::WALL, this,
            trackPhys);

    if (LoadChainFromFile("Assets/ColliderPoints/InternalTrackPointsS2.txt", internalTrackPointsS2))
        trackIntS2 = new Colliders(
            0, 0,
            internalTrackPointsS2.data(),
            internalTrackPointsS2.size(),
            ColliderType::WALL, this,
            trackPhys);

    if (LoadChainFromFile("Assets/ColliderPoints/SensorTrackPointsAboveRight.txt", sensorTrackPointsAboveRight))
        sensorAboveRight = new Colliders(
            0, 0,
            sensorTrackPointsAboveRight.data(),
            sensorTrackPointsAboveRight.size(),
            ColliderType::WALL, this,
            trackPhys);

    if (LoadChainFromFile("Assets/ColliderPoints/SensorTrackPointsAboveLeft.txt", sensorTrackPointsAboveLeft))
        sensorAboveLeft = new Colliders(
            0, 0,
            sensorTrackPointsAboveLeft.data(),
            sensorTrackPointsAboveLeft.size(),
            ColliderType::WALL, this,
            trackPhys);

    if (LoadChainFromFile("Assets/ColliderPoints/SensorTrackPointsBelowUp.txt", sensorTrackPointsBelowUp))
        sensorBelowUp = new Colliders(
            0, 0,
            sensorTrackPointsBelowUp.data(),
            sensorTrackPointsBelowUp.size(),
            ColliderType::WALL, this,
            trackPhys);

    if (LoadChainFromFile("Assets/ColliderPoints/SensorTrackPointsBelowDown.txt", sensorTrackPointsBelowDown))
        sensorBelowDown = new Colliders(
            0, 0,
            sensorTrackPointsBelowDown.data(),
            sensorTrackPointsBelowDown.size(),
            ColliderType::WALL, this,
            trackPhys);
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