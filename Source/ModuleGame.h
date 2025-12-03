#pragma once

#include "Globals.h"
#include "Module.h"
#include "Car.h"
#include "Player.h"
#include "AICar.h"

#include "p2Point.h"

#include "raylib.h"
#include <vector>

enum GameState
{
	InitialMenu,
	Gameplay
};

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

	void OnCollision(PhysBody* physA, PhysBody* physB) override;
	void EndCollision(PhysBody* physA, PhysBody* physB) override;

private:
	void DrawGameplay();
	void DrawInitialMenu();

	void InitialMenu(float dt);
	void Gameplay(float dt);

	void CarsUpdate(float dt);
	void CarsDraw();

public:
	GameState gameState;

private:
	Car car;
	Player player;
	std::vector<AICar*> aiCars;

	float time = 0.0f;

	PhysBody* pAMR23;
	PhysBody* pR25;
	PhysBody* pGP2Engine;
	PhysBody* pMc4;
	PhysBody* pMc22;
	PhysBody* pPinkMerc;
	PhysBody* pW11;
	PhysBody* pRB21; 

	Texture2D tAMR23;
	Texture2D tR25;
	Texture2D tGP2Engine;
	Texture2D tMc4;
	Texture2D tMc22;
	Texture2D tPinkMerc;
	Texture2D tW11;
	Texture2D tRB21;
};