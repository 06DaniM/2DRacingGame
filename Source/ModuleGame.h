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

class ModuleGame : public Module, public Listener
{
public:
	ModuleGame(Application* app, bool start_enabled = true);
	~ModuleGame();

	bool Start();
	update_status Update();
	bool CleanUp();

	void OnCollision(PhysBody* physA, PhysBody* physB);
	void EndCollision(PhysBody* physA, PhysBody* physB);

private:
	void DrawGameplay();
	void DrawInitialMenu();

	void InitialMenu(float dt);
	void Gameplay(float dt);

	void GameplayStart();

	void CarsUpdate(float dt);
	void CarsDraw();

public:
	GameState gameState;

private:
	Car car;
	Player player;
	std::vector<AICar*> aiCars;
	std::vector<PhysBody*> carsPhys;

	float time = 0.0f;

	PhysBody* pAMR23 = NULL;
	PhysBody* pR25 = NULL;
	PhysBody* pGP2Engine = NULL;
	PhysBody* pMp4 = NULL;
	PhysBody* pMp22 = NULL;
	PhysBody* pPinkMerc = NULL;
	PhysBody* pW11 = NULL;
	PhysBody* pRB21 = NULL;

	Texture2D tAMR23;
	Texture2D tR25;
	Texture2D tGP2Engine;
	Texture2D tMp4;
	Texture2D tMp22;
	Texture2D tPinkMerc;
	Texture2D tW11;
	Texture2D tRB21;

	PhysBody* checkeredFlag = NULL;

	Texture2D track;

	bool gamePlayStart = false;
};