#pragma once

#include "Globals.h"
#include "Module.h"
#include "Player.h"
#include "AICar.h"

#include "p2Point.h"

#include "raylib.h"
#include <vector>

class PhysBody;
class PhysicEntity;

class ModuleGame : public Module
{
public:
	ModuleGame(Application* app, bool start_enabled = true);
	~ModuleGame();

	bool Start();
	update_status Update();
	bool CleanUp();

private:
	Player player;
	std::vector<AICar*> aiCars;
};
