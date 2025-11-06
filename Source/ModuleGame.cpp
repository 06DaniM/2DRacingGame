#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleGame.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"

ModuleGame::ModuleGame(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	
}

ModuleGame::~ModuleGame()
{}

bool ModuleGame::Start()
{
    player.Start({ SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 });

    for (int i = 0; i < 3; ++i)
    {
        AICar* ai = new AICar();
        ai->Start({ (float)SCREEN_WIDTH / 2 + i * 60, SCREEN_HEIGHT / 2 });
        aiCars.push_back(ai);
    }

    return true;
}

update_status ModuleGame::Update()
{
    float dt = GetFrameTime();

    player.Update(dt);

    for (auto ai : aiCars)
        ai->Update(dt);

    player.Draw();
    for (auto ai : aiCars)
        ai->Draw();

    return UPDATE_CONTINUE;
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
    return true;
}

void ModuleGame::OnCollision(PhysBody* physA, PhysBody* physB)
{
}

void ModuleGame::EndCollision(PhysBody* physA, PhysBody* physB)
{
}