#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleGame.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"
#include <vector>
#include <algorithm>

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

    App->renderer->DrawInsideCamera = [this]() { if (gameState == GameState::Gameplay) DrawGameplay(); };
    App->renderer->DrawAfterBegin = [this]() { DrawUI(); };

    gameState = GameState::InitialMenu;

    return true;
}

update_status ModuleGame::Update()
{
    if (IsKeyPressed(KEY_F5)) player.lap = 5;
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
                PhysBody* car = (PhysBody*)fixture->GetBody()->GetUserData().pointer;

                if (!car->id.empty()) 
                {
                    player.texture = car->carTexture;
                    playerIdSelected = car->id;

                    car->selectable = false;

                    gameState = GameState::Gameplay;
                }
            }
        }
    }

    DrawInitialMenu();
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
    if (initialMenuStart) return;
    gamePlayStart = false;

    auto createUIRect = [this](float x, float y, Texture2D tex, std::string id)
        {
            PhysBody* p = App->physics->CreateRectangle(x, y, 29 * 2, 73 * 2, 0.0f, true, 0, ColliderType::UI, STATIC);
            p->id = id;
            p->carTexture = tex;
            carsPhys.push_back(p);
            return p;
        };

    pAMR23 = createUIRect(SCREEN_WIDTH * 0.26 + 1, SCREEN_HEIGHT / 2 * 0.65, tAMR23, "AMR23");
    pGP2Engine = createUIRect(SCREEN_WIDTH * 0.42 + 1, SCREEN_HEIGHT / 2 * 0.65, tGP2Engine, "GP2");
    pW11 = createUIRect(SCREEN_WIDTH * 0.58 + 1, SCREEN_HEIGHT / 2 * 0.65, tW11, "W11");
    pPinkMerc = createUIRect(SCREEN_WIDTH * 0.74 + 1, SCREEN_HEIGHT / 2 * 0.65, tPinkMerc, "PinkMerc");
    pR25 = createUIRect(SCREEN_WIDTH * 0.26 + 1, SCREEN_HEIGHT / 2 * 1.35, tR25, "R25");
    pMp4 = createUIRect(SCREEN_WIDTH * 0.42 + 1, SCREEN_HEIGHT / 2 * 1.35, tMp4, "Mc4");
    pMp22 = createUIRect(SCREEN_WIDTH * 0.58 + 1, SCREEN_HEIGHT / 2 * 1.35, tMp22, "Mc22");
    pRB21 = createUIRect(SCREEN_WIDTH * 0.74 + 1, SCREEN_HEIGHT / 2 * 1.35, tRB21, "RB21");

    initialMenuStart = true;
}

void ModuleGame::GameplayStart()
{
    if (gamePlayStart) return;

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

    // Sets the textures for the ai cars
    for (auto ai : aiCars) {
        for (auto phys : carsPhys) {
            if (phys->selectable) {
                ai->texture = phys->carTexture;
                ai->pbody->id = phys->id;
                phys->selectable = false;
                break;
            }
        }
    }

    // Destroy the physbodies of the cars to select
    for (auto p : carsPhys) App->physics->DestroyBody(p);
    carsPhys.clear();

    // Creation of the checkered flags
    checkeredFlag = App->physics->CreateRectangle(600, 500, 20, 280, 0.0f, true, this, ColliderType::CHECKEREDFLAG, STATIC);

    checkpoints.push_back(new Checkpoint(1249, 1220, 20, 270, 1, 45, this));
    checkpoints.push_back(new Checkpoint(2051, 1589, 20, 270, 2, 0, this));
    checkpoints.push_back(new Checkpoint(2566, 1185, 20, 260, 3, 120, this));
    checkpoints.push_back(new Checkpoint(2694, 1494, 20, 250, 4, 100, this));
    checkpoints.push_back(new Checkpoint(2758, 2489, 20, 250, 5, 90, this));
    checkpoints.push_back(new Checkpoint(3736, 2217, 20, 250, 6, 110, this));
    checkpoints.push_back(new Checkpoint(4629, 2516, 20, 290, 7, 90, this));
    checkpoints.push_back(new Checkpoint(6066, 4180, 20, 270, 8, 30, this));
    checkpoints.push_back(new Checkpoint(5307, 2155, 20, 280, 9, 60, this));
    checkpoints.push_back(new Checkpoint(3869, 1599, 20, 290, 10, -50, this));
    checkpoints.push_back(new Checkpoint(2388, 2089, 20, 270, 11, 20, this));
    checkpoints.push_back(new Checkpoint(442, 1076, 20, 270, 12, 55, this));

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
    player.canMove = true; // quitar
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

            for (auto ch : checkpoints)
                ch->~Checkpoint();

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

        // Draw the UI in gameplay
        DrawText(TextFormat("Lap Time: %.2f", player.currentLapTime), 20, 50, 20, BLACK);
        DrawText(TextFormat("Previous Lap Time: %.2f", player.previousLapTime), 20, 70, 20, BLACK);
        DrawText(TextFormat("Fastest Lap Time: %.2f", player.fastestLapTime), 20, 120, 20, BLACK);
        DrawText(TextFormat("Lap: %d", showLap), SCREEN_WIDTH - 100, 50, 20, BLACK);
        return;
    }

    else
    {
        // Draw the UI after gameplay
        DrawText(TextFormat("%d Laps", player.totalLaps), SCREEN_WIDTH / 2 - 80, 150, 50, BLACK);
        DrawText(TextFormat("Fastest Lap: %.2f", player.fastestLapTime), SCREEN_WIDTH / 2 - 200, 200, 50, BLACK);
        return;
    }
}

void ModuleGame::DrawInitialMenu()
{
    // Row 1
    DrawTextureEx(tAMR23, { SCREEN_WIDTH * 0.26f - 29, SCREEN_HEIGHT / 2 * 0.65f + 73 }, -90, 2, WHITE);
    DrawTextureEx(tGP2Engine, { SCREEN_WIDTH * 0.42f - 29, SCREEN_HEIGHT / 2 * 0.65f + 73 }, -90, 2, WHITE);
    DrawTextureEx(tW11, { SCREEN_WIDTH * 0.58f - 29, SCREEN_HEIGHT / 2 * 0.65f + 73 }, -90, 2, WHITE);
    DrawTextureEx(tPinkMerc, { SCREEN_WIDTH * 0.74f - 29, SCREEN_HEIGHT / 2 * 0.65f + 73 }, -90, 2, WHITE);

    // Row 2
    DrawTextureEx(tR25, { SCREEN_WIDTH * 0.26f - 29, SCREEN_HEIGHT / 2 * 1.35f + 73 }, -90, 2, WHITE);
    DrawTextureEx(tMp4, { SCREEN_WIDTH * 0.42f - 29, SCREEN_HEIGHT / 2 * 1.35f + 73 }, -90, 2, WHITE);
    DrawTextureEx(tMp22, { SCREEN_WIDTH * 0.58f - 29, SCREEN_HEIGHT / 2 * 1.35f + 73 }, -90, 2, WHITE);
    DrawTextureEx(tRB21, { SCREEN_WIDTH * 0.74f - 29, SCREEN_HEIGHT / 2 * 1.35f + 73 }, -90, 2, WHITE);
}

void ModuleGame::CarsDraw()
{
    // Draw the cars
    player.Draw();
    for (auto ai : aiCars) ai->Draw();
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
                if (player.fastestLapTime > player.currentLapTime) 
                    player.fastestLapTime = player.currentLapTime;
                player.currentLapTime = 0.0f;
            }
        }

        else if (physA->ctype == ColliderType::CHECKPOINT)
        {
            if (player.checkpoint + 1 == physA->n)
                player.checkpoint++;
        }
        break;

    case ColliderType::AICAR:
        if (physA->ctype == ColliderType::CHECKPOINT)
        {
            
            auto car = dynamic_cast<AICar*>(physB->listener);

            if (car != NULL)
                if (car->checkpoint + 1 == physA->n)
                    car->checkpoint++;
        }
        break;


    default:
        break;
    }
}

void ModuleGame::EndCollision(PhysBody* physA, PhysBody* physB)
{
}