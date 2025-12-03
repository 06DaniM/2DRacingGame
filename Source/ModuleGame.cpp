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

    auto createUIRect = [this](float x, float y, Texture2D tex, const char* id) 
        {
        PhysBody* p = App->physics->CreateRectangle(x, y, 29 * 2, 73 * 2, true, 0, ColliderType::UI, STATIC);
        p->id = id;
        p->carTexture = tex;
        carsPhys.push_back(p);
        return p;
        };

    pAMR23      = createUIRect(SCREEN_WIDTH * 0.26 + 1, SCREEN_HEIGHT / 2 * 0.65, tAMR23,       "AMR23");
    pGP2Engine  = createUIRect(SCREEN_WIDTH * 0.42 + 1, SCREEN_HEIGHT / 2 * 0.65, tGP2Engine,   "GP2");
    pW11        = createUIRect(SCREEN_WIDTH * 0.58 + 1, SCREEN_HEIGHT / 2 * 0.65, tW11,         "W11");
    pPinkMerc   = createUIRect(SCREEN_WIDTH * 0.74 + 1, SCREEN_HEIGHT / 2 * 0.65, tPinkMerc,    "PinkMerc");
    pR25        = createUIRect(SCREEN_WIDTH * 0.26 + 1, SCREEN_HEIGHT / 2 * 1.35, tR25,         "R25");
    pMp4        = createUIRect(SCREEN_WIDTH * 0.42 + 1, SCREEN_HEIGHT / 2 * 1.35, tMp4,         "Mc4");
    pMp22       = createUIRect(SCREEN_WIDTH * 0.58 + 1, SCREEN_HEIGHT / 2 * 1.35, tMp22,        "Mc22");
    pRB21       = createUIRect(SCREEN_WIDTH * 0.74 + 1, SCREEN_HEIGHT / 2 * 1.35, tRB21,        "RB21");

    App->renderer->DrawInsideCamera = [this]() { DrawGameplay(); };
    App->renderer->DrawAfterBegin = [this]() { DrawUI(); };

    gameState = GameState::InitialMenu;

    return true;
}

update_status ModuleGame::Update()
{
    float dt = GetFrameTime();

    switch (gameState)
    {
    case GameState::InitialMenu: InitialMenu(dt); break;
    case GameState::Gameplay:    Gameplay(dt); break;
    }

    return UPDATE_CONTINUE;
}

// === INITIAL MENU === 
void ModuleGame::InitialMenu(float dt)
{
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mouse = GetMousePosition();
        b2Vec2 pMousePos = { PIXELS_TO_METERS(mouse.x), PIXELS_TO_METERS(mouse.y) };

        for (auto fixture : App->physics->GetFixtures()) {
            if (fixture->TestPoint(pMousePos)) {
                PhysBody* car = (PhysBody*)fixture->GetBody()->GetUserData().pointer;
                if (!car->id.empty()) {
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
    time += dt;

    GameplayStart();
    CarsUpdate(dt);
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

    checkeredFlag = App->physics->CreateRectangle(600, 500, 20, 250, true, this, ColliderType::CHECKEREDFLAG, STATIC);
    gamePlayStart = true;
}

// === CARS UPDATE ===
void ModuleGame::CarsUpdate(float dt)
{
    player.Update(dt);
    for (auto ai : aiCars) ai->Update(dt);
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
    // Draw the initial menu
    if (gameState == GameState::InitialMenu) {
        DrawInitialMenu();
        return;
    }

    // Draw the UI in gameplay
    DrawText(TextFormat("Time: %.2f", time), 20, 50, 20, BLACK);
    DrawText(TextFormat("Lap: %d", player.lap), 20, 70, 20, BLACK);
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
            player.lap++;
        }
        break;

    default:
        break;
    }
}

void ModuleGame::EndCollision(PhysBody* physA, PhysBody* physB)
{
}