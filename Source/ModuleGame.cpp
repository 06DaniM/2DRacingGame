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
    tMp4 = LoadTexture("Assets/Textures/Mc4.png");
    tMp22 = LoadTexture("Assets/Textures/Mc22.png");
    tPinkMerc = LoadTexture("Assets/Textures/PinkMerc.png");
    tW11 = LoadTexture("Assets/Textures/W11.png");
    tRB21 = LoadTexture("Assets/Textures/RedBull(Tututuru).png");

    track = LoadTexture("Assets/Textures/Track.png");

    pAMR23 = App->physics->CreateRectangle(SCREEN_WIDTH * 0.26 + 1, SCREEN_HEIGHT / 2 * 0.65, 29 * 2, 73 * 2, true, 0, ColliderType::UI, STATIC);
    pAMR23->id = "AMR23";
    pAMR23->carTexture = tAMR23;
    carsPhys.push_back(pAMR23);

    pGP2Engine = App->physics->CreateRectangle(SCREEN_WIDTH * 0.42 + 1, SCREEN_HEIGHT / 2 * 0.65, 29 * 2, 73 * 2, true, 0, ColliderType::UI, STATIC);
    pGP2Engine->id = "GP2";
    pGP2Engine->carTexture = tGP2Engine;
    carsPhys.push_back(pGP2Engine);

    pW11 = App->physics->CreateRectangle(SCREEN_WIDTH * 0.58 + 1, SCREEN_HEIGHT / 2 * 0.65, 29 * 2, 73 * 2, true, 0, ColliderType::UI, STATIC);
    pW11->id = "W11";
    pW11->carTexture = tW11;
    carsPhys.push_back(pW11);
    
    pPinkMerc = App->physics->CreateRectangle(SCREEN_WIDTH * 0.74 + 1, SCREEN_HEIGHT / 2 * 0.65, 29 * 2, 73 * 2, true, 0, ColliderType::UI, STATIC);
    pPinkMerc->id = "PinkMerc";
    pPinkMerc->carTexture = tPinkMerc;
    carsPhys.push_back(pPinkMerc);

    pR25 = App->physics->CreateRectangle(SCREEN_WIDTH * 0.26 + 1, SCREEN_HEIGHT / 2 * 1.35, 29 * 2, 73 * 2, true, 0, ColliderType::UI, STATIC);
    pR25->id = "R25";
    pR25->carTexture = tR25;
    carsPhys.push_back(pR25);

    pMp4 = App->physics->CreateRectangle(SCREEN_WIDTH * 0.42 + 1, SCREEN_HEIGHT / 2 * 1.35, 29 * 2, 73 * 2, true, 0, ColliderType::UI, STATIC);
    pMp4->id = "Mc4";
    pMp4->carTexture = tMp4;
    carsPhys.push_back(pMp4);

    pMp22 = App->physics->CreateRectangle(SCREEN_WIDTH * 0.58 + 1, SCREEN_HEIGHT / 2 * 1.35, 29 * 2, 73 * 2, true, 0, ColliderType::UI, STATIC);
    pMp22->id = "Mc22";
    pMp22->carTexture = tMp22;
    carsPhys.push_back(pMp22);

    pRB21 = App->physics->CreateRectangle(SCREEN_WIDTH * 0.74 + 1, SCREEN_HEIGHT / 2 * 1.35, 29 * 2, 73 * 2, true, 0, ColliderType::UI, STATIC);
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
    DrawTextureEx(tAMR23, { SCREEN_WIDTH * 0.26 - 29, SCREEN_HEIGHT / 2 * 0.65 + 73}, -90, 2, WHITE);
    DrawTextureEx(tGP2Engine, { SCREEN_WIDTH * 0.42 - 29, SCREEN_HEIGHT / 2 * 0.65 + 73 }, -90, 2, WHITE);
    DrawTextureEx(tW11, { SCREEN_WIDTH * 0.58 - 29, SCREEN_HEIGHT / 2 * 0.65 + 73 }, -90, 2, WHITE);
    DrawTextureEx(tPinkMerc, { SCREEN_WIDTH * 0.74 - 29, SCREEN_HEIGHT / 2 * 0.65 + 73 }, -90, 2, WHITE);

    // Row 2
    DrawTextureEx(tR25, { SCREEN_WIDTH * 0.26 - 29, SCREEN_HEIGHT / 2 * 1.35 + 73}, -90, 2, WHITE);
    DrawTextureEx(tMp4, { SCREEN_WIDTH * 0.42 - 29, SCREEN_HEIGHT / 2 * 1.35 + 73 }, -90, 2, WHITE);
    DrawTextureEx(tMp22, { SCREEN_WIDTH * 0.58 - 29, SCREEN_HEIGHT / 2 * 1.35 + 73 }, -90, 2, WHITE);
    DrawTextureEx(tRB21, { SCREEN_WIDTH * 0.74 - 29, SCREEN_HEIGHT / 2 * 1.35 + 73 }, -90, 2, WHITE);
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
    App->physics->DestroyBody(pMp22);
    App->physics->DestroyBody(pMp4);
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

    checkeredFlag = App->physics->CreateRectangle(200, 300, 20, 100, true, this, ColliderType::CHECKEREDFLAG, STATIC);
    
    gamePlayStart = true;
}

void ModuleGame::DrawGameplay()
{
    DrawTextureEx(track, { 0,-100 }, 0, 5.0f, WHITE);

    // Draw the cars
    CarsDraw();

    // Draw the UI
    DrawText(TextFormat("Time: %f", time), 20, 50, 20, BLACK);
    DrawText(TextFormat("Lap: %d", player.lap), 20, 70, 20, BLACK);
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
    UnloadTexture(tMp22);
    UnloadTexture(tMp4);
    UnloadTexture(tPinkMerc);
    UnloadTexture(tR25);
    UnloadTexture(tRB21);

    return true;
}

void ModuleGame::OnCollision(PhysBody* physA, PhysBody* physB)
{
    switch (physA->ctype)
    {
    case ColliderType::PLAYER:
        if (physB->ctype == ColliderType::CHECKEREDFLAG)
        {
            LOG("Collision with checkered flag");
        }

    default:
        break;
    }
}

void ModuleGame::EndCollision(PhysBody* physA, PhysBody* physB)
{
}