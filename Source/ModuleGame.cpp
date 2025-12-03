#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleGame.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"

#include <vector>

ModuleGame::ModuleGame(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	
}

ModuleGame::~ModuleGame()
{}

bool ModuleGame::Start()
{
    SetTargetFPS(60);

    tAMR23 = LoadTexture("Assets/Textures/AMR23.png");
    tR25 = LoadTexture("Assets/Textures/R25.png");
    tGP2Engine = LoadTexture("Assets/Textures/GP2Engine.png");
    tMc4 = LoadTexture("Assets/Textures/Mc4.png");
    tMc22 = LoadTexture("Assets/Textures/Mc22.png");
    tPinkMerc = LoadTexture("Assets/Textures/PinkMerc.png");
    tW11 = LoadTexture("Assets/Textures/W11.png");
    tRB21 = LoadTexture("Assets/Textures/RedBull(Tututuru).png");

    pAMR23 = App->physics->CreateRectangle(SCREEN_WIDTH * 0.26 + 30, SCREEN_HEIGHT / 2 * 0.65, 29 * 2, 73 * 2, true, 0, ColliderType::UI, STATIC);
    pAMR23->id = "AMR23";
    pAMR23->carTexture = tAMR23;
    carsPhys.push_back(pAMR23);

    pGP2Engine = App->physics->CreateRectangle(SCREEN_WIDTH * 0.42 + 30, SCREEN_HEIGHT / 2 * 0.65, 29 * 2, 73 * 2, true, 0, ColliderType::UI, STATIC);
    pGP2Engine->id = "GP2";
    pGP2Engine->carTexture = tGP2Engine;
    carsPhys.push_back(pGP2Engine);

    pW11 = App->physics->CreateRectangle(SCREEN_WIDTH * 0.58 + 30, SCREEN_HEIGHT / 2 * 0.65, 29 * 2, 73 * 2, true, 0, ColliderType::UI, STATIC);
    pW11->id = "W11";
    pW11->carTexture = tW11;
    carsPhys.push_back(pW11);
    
    pPinkMerc = App->physics->CreateRectangle(SCREEN_WIDTH * 0.74 + 30, SCREEN_HEIGHT / 2 * 0.65, 29 * 2, 73 * 2, true, 0, ColliderType::UI, STATIC);
    pPinkMerc->id = "PinkMerc";
    pPinkMerc->carTexture = tPinkMerc;
    carsPhys.push_back(pPinkMerc);

    pR25 = App->physics->CreateRectangle(SCREEN_WIDTH * 0.26 + 30, SCREEN_HEIGHT / 2 * 1.35, 29 * 2, 73 * 2, true, 0, ColliderType::UI, STATIC);
    pR25->id = "R25";
    pR25->carTexture = tR25;
    carsPhys.push_back(pR25);

    pMc4 = App->physics->CreateRectangle(SCREEN_WIDTH * 0.42 + 30, SCREEN_HEIGHT / 2 * 1.35, 29 * 2, 73 * 2, true, 0, ColliderType::UI, STATIC);
    pMc4->id = "Mc4";
    pMc4->carTexture = tMc4;
    carsPhys.push_back(pMc4);

    pMc22 = App->physics->CreateRectangle(SCREEN_WIDTH * 0.58 + 30, SCREEN_HEIGHT / 2 * 1.35, 29 * 2, 73 * 2, true, 0, ColliderType::UI, STATIC);
    pMc22->id = "Mc22";
    pMc22->carTexture = tMc22;
    carsPhys.push_back(pMc22);

    pRB21 = App->physics->CreateRectangle(SCREEN_WIDTH * 0.74 + 30, SCREEN_HEIGHT / 2 * 1.35, 29 * 2, 73 * 2, true, 0, ColliderType::UI, STATIC);
    pRB21->id = "RB21";
    pRB21->carTexture = tRB21;
    carsPhys.push_back(pRB21);

    player.Start({ SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 });

    for (int i = 0; i < 7; ++i)
    {
        AICar* ai = new AICar();
        ai->Start({ (float)SCREEN_WIDTH / 2 + i * 100, SCREEN_HEIGHT / 2 });
        aiCars.push_back(ai);
    }

    gameState = GameState::InitialMenu;

    return true;
}

update_status ModuleGame::Update()
{
    float dt = GetFrameTime();

    switch (gameState)
    {
    case GameState::InitialMenu:
        InitialMenu(dt);
        break;

    case GameState::Gameplay:
        Gameplay(dt);
        break;
    default:
        break;
    }

    return UPDATE_CONTINUE;
}

void ModuleGame::DrawInitialMenu()
{
    // Row 1
    DrawTextureEx(tAMR23, { SCREEN_WIDTH * 0.26, SCREEN_HEIGHT / 2 * 0.65 + 73}, -90, 2, WHITE);
    DrawTextureEx(tGP2Engine, { SCREEN_WIDTH * 0.42, SCREEN_HEIGHT / 2 * 0.65 + 73 }, -90, 2, WHITE);
    DrawTextureEx(tW11, { SCREEN_WIDTH * 0.58, SCREEN_HEIGHT / 2 * 0.65 + 73 }, -90, 2, WHITE);
    DrawTextureEx(tPinkMerc, { SCREEN_WIDTH * 0.74, SCREEN_HEIGHT / 2 * 0.65 + 73 }, -90, 2, WHITE);

    // Row 2
    DrawTextureEx(tR25, { SCREEN_WIDTH * 0.26, SCREEN_HEIGHT / 2 * 1.35 + 73}, -90, 2, WHITE);
    DrawTextureEx(tMc4, { SCREEN_WIDTH * 0.42, SCREEN_HEIGHT / 2 * 1.35 + 73 }, -90, 2, WHITE);
    DrawTextureEx(tMc22, { SCREEN_WIDTH * 0.58, SCREEN_HEIGHT / 2 * 1.35 + 73 }, -90, 2, WHITE);
    DrawTextureEx(tRB21, { SCREEN_WIDTH * 0.74, SCREEN_HEIGHT / 2 * 1.35 + 73 }, -90, 2, WHITE);
}

void ModuleGame::InitialMenu(float dt)
{
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        Vector2 mouse = GetMousePosition();
        b2Vec2 pMousePosition = b2Vec2(PIXELS_TO_METERS(mouse.x), PIXELS_TO_METERS(mouse.y));
        std::vector<b2Fixture*> fixtures = App->physics->GetFixtures();

        for (auto fixture : fixtures)
        {
            if (fixture->TestPoint(pMousePosition))
            {
                PhysBody* car = (PhysBody*)fixture->GetBody()->GetUserData().pointer;

                if (car->id != "")
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

void ModuleGame::Gameplay(float dt)
{
    time += dt;

    GameplayStart();
    CarsUpdate(dt);
    DrawGameplay();
}

void ModuleGame::GameplayStart()
{
    if (gamePlayStart) return;

    App->physics->DestroyBody(pAMR23);
    App->physics->DestroyBody(pGP2Engine);
    App->physics->DestroyBody(pMc22);
    App->physics->DestroyBody(pMc4);
    App->physics->DestroyBody(pPinkMerc);
    App->physics->DestroyBody(pR25);
    App->physics->DestroyBody(pW11);
    App->physics->DestroyBody(pRB21);

    for (auto ai : aiCars)
    {
        for (auto phys : carsPhys)
        {
            if (phys->selectable)
            {
                ai->texture = phys->carTexture;
                phys->selectable = false;
                break;
            }
        }
    }
    
    gamePlayStart = true;
}

void ModuleGame::DrawGameplay()
{
    // Draw the UI
    DrawText(TextFormat("Time: %f", time), 20, 50, 20, BLACK);

    // Draw the cars
    CarsDraw();
}

void ModuleGame::CarsUpdate(float dt)
{
    // Cars Update
    player.Update(dt);

    for (auto ai : aiCars)
        ai->Update(dt);
}

void ModuleGame::CarsDraw()
{
    // Cars draw
    player.Draw();
    for (auto ai : aiCars)
        ai->Draw();
}

bool ModuleGame::CleanUp()
{
    player.CleanUp();
    for (auto ai : aiCars)
    {
        ai->CleanUp();
        delete ai;
    }
    aiCars.clear();

    UnloadTexture(tAMR23);
    UnloadTexture(tGP2Engine);
    UnloadTexture(tW11);
    UnloadTexture(tMc22);
    UnloadTexture(tMc4);
    UnloadTexture(tPinkMerc);
    UnloadTexture(tR25);
    UnloadTexture(tRB21);

    return true;
}

void ModuleGame::OnCollision(PhysBody* physA, PhysBody* physB)
{
}

void ModuleGame::EndCollision(PhysBody* physA, PhysBody* physB)
{
}