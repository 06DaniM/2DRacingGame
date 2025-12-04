#pragma once

#include "Globals.h"
#include "Module.h"
#include "Car.h"
#include "Player.h"
#include "AICar.h"
#include "Listener.h"

#include "p2Point.h"
#include "Coroutine.h"

#include "raylib.h"
#include <vector>

enum GameState
{
	InitialMenu,
	Gameplay,
	EndGame
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

	void OnCollision(PhysBody* physA, PhysBody* physB) override;
	void EndCollision(PhysBody* physA, PhysBody* physB) override;

private:
	void DrawGameplay();
	void DrawInitialMenu();

	void InitialMenu(float dt);
	void Gameplay(float dt);
	void EndGameMenu(float dt);

	void DrawUI();

	void InitialMenuStart();
	void GameplayStart();
	void TrafficLight();

	void CarsUpdate(float dt);
	void GameManager(float dt);

	void CarsDraw();

	void Destroy(PhysBody* pbody)
	{
		App->physics->DestroyBody(pbody);
	}

public:
	GameState gameState;

private:
	Car car;
	Player player;
	std::vector<AICar*> aiCars;
	CoroutineManager coroutineManager;

	std::vector<PhysBody*> carsPhys;
	std::vector<PhysBody*> checkPhys;

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

	PhysBody* checkPoint1 = NULL;
	PhysBody* checkPoint2 = NULL;
	PhysBody* checkPoint3 = NULL;
	PhysBody* checkPoint4 = NULL;
	PhysBody* checkPoint5 = NULL;
	PhysBody* checkPoint6 = NULL;
	PhysBody* checkPoint7 = NULL;
	PhysBody* checkPoint8 = NULL;
	PhysBody* checkPoint9 = NULL;
	PhysBody* checkPoint10 = NULL;
	PhysBody* checkPoint11 = NULL;
	PhysBody* checkPoint12 = NULL;

	Texture2D track;

	bool initialMenuStart = false;
	bool gamePlayStart = false;
	bool lightsOut = false;

	float lightTimer = 0.0f;

	int showLap = 1;

	Camera2D camera = { 0 };
};