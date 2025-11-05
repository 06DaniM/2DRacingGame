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
    player.Start();

    for (int i = 0; i < 3; ++i)
    {
        AICar* ai = new AICar();
        ai->Start();
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

