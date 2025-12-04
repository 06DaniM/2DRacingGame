#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleGame.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"
#include <vector>

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
    time += dt;
    if (time >= 1.0f)
    {
        gameState = GameState::InitialMenu;
    }
}

void ModuleGame::InitialMenuStart()
{
    if (initialMenuStart) return;
    gamePlayStart = false;

    auto createUIRect = [this](float x, float y, Texture2D tex, const char* id)
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

    for (int i = 0; i < 7; ++i)
    {
        AICar* ai = new AICar();
        ai->Start({ (float)SCREEN_WIDTH / 2 + i * 100, SCREEN_HEIGHT / 2 });
        aiCars.push_back(ai);
    }

    // Sets the player for the camera
    App->renderer->SetPlayer(&player);

    // Destroy the physbodies of the cars to select
    for (auto p : carsPhys) App->physics->DestroyBody(p);

    // Sets the textures for the ai cars
    for (auto ai : aiCars) {
        for (auto phys : carsPhys) {
            if (phys->selectable) {
                ai->texture = phys->carTexture;
                phys->selectable = false;
                break;
            }
        }
    }
    carsPhys.clear();

    checkeredFlag = App->physics->CreateRectangle(600, 500, 20, 280, 0.0f, true, this, ColliderType::CHECKEREDFLAG, STATIC);
    checkPhys.push_back(checkeredFlag);

    checkPoint1 = App->physics->CreateRectangle(1249, 1220, 20, 270, 45.0f, true, this, ColliderType::CHECKPOINT, STATIC);
    checkPoint1->n = 1;
    checkPhys.push_back(checkPoint1);

    checkPoint2 = App->physics->CreateRectangle(2051, 1589, 20, 270, 0.0f, true, this, ColliderType::CHECKPOINT, STATIC);
    checkPoint2->n = 2;
    checkPhys.push_back(checkPoint2);

    checkPoint3 = App->physics->CreateRectangle(2566, 1185, 20, 260, 120.0f, true, this, ColliderType::CHECKPOINT, STATIC);
    checkPoint3->n = 3;
    checkPhys.push_back(checkPoint3);

    checkPoint4 = App->physics->CreateRectangle(2694, 1494, 20, 250, 100.0f, true, this, ColliderType::CHECKPOINT, STATIC);
    checkPoint4->n = 4;
    checkPhys.push_back(checkPoint4);

    checkPoint5 = App->physics->CreateRectangle(2758, 2489, 20, 250, 90.0f, true, this, ColliderType::CHECKPOINT, STATIC);
    checkPoint5->n = 5;
    checkPhys.push_back(checkPoint5);

    checkPoint6 = App->physics->CreateRectangle(3736, 2217, 20, 250, 110.0f, true, this, ColliderType::CHECKPOINT, STATIC);
    checkPoint6->n = 6;
    checkPhys.push_back(checkPoint6);

    checkPoint7 = App->physics->CreateRectangle(4629, 2516, 20, 290, 90.0f, true, this, ColliderType::CHECKPOINT, STATIC);
    checkPoint7->n = 7;
    checkPhys.push_back(checkPoint7);

    checkPoint8 = App->physics->CreateRectangle(6066, 4180, 20, 270, 30.0f, true, this, ColliderType::CHECKPOINT, STATIC);
    checkPoint8->n = 8;
    checkPhys.push_back(checkPoint8);

    checkPoint9 = App->physics->CreateRectangle(5307, 2155, 20, 280, 60.0f, true, this, ColliderType::CHECKPOINT, STATIC);
    checkPoint9->n = 9;
    checkPhys.push_back(checkPoint9);

    checkPoint10 = App->physics->CreateRectangle(3869, 1599, 20, 290, -50.0f, true, this, ColliderType::CHECKPOINT, STATIC);
    checkPoint10->n = 10;
    checkPhys.push_back(checkPoint10);

    checkPoint11 = App->physics->CreateRectangle(2388, 2089, 20, 270, 20.0f, true, this, ColliderType::CHECKPOINT, STATIC);
    checkPoint11->n = 11;
    checkPhys.push_back(checkPoint11);

    checkPoint12 = App->physics->CreateRectangle(442, 1076, 20, 270, 55.0f, true, this, ColliderType::CHECKPOINT, STATIC);
    checkPoint12->n = 12;
    checkPhys.push_back(checkPoint12);

    time = 0.0f;

    gamePlayStart = true;
    initialMenuStart = false;
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
        if (player.lap > 0 && player.lap)
        {
            player.currentLapTime += GetFrameTime();
            showLap = player.lap;
        }
    }

    else
    {
        time += dt;
        if (time >= 1.0f)
        {
            for (auto ai : aiCars)
            {
                ai->Destroy();
            }
            aiCars.clear();
            player.Destroy();

            for (auto ch : checkPhys)
                App->physics->DestroyBody(ch);
            checkPhys.clear();

            time = 0;

            gameState = GameState::EndGame;
        }
    }
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
            if (player.checkpoint == 12 || player.lap == 0)
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
            {
                LOG("Checkpoint detected");
                player.checkpoint++;
            }
        }
        break;

    default:
        break;
    }
}

void ModuleGame::EndCollision(PhysBody* physA, PhysBody* physB)
{
}