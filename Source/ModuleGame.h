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

class PhysBody;
class PhysicEntity;

enum GameState
{
	InitialMenu,
	Gameplay,
	EndGame
};

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
	void UpdatePosition();

	void CarsDraw();

	void Destroy(PhysBody* pbody)
	{
		App->physics->DestroyBody(pbody);
	}

public:
	GameState gameState;

	std::vector<Checkpoint*> checkpoints;	// Vector with the checkpoints
	PhysBody* checkeredFlag = NULL;			// Physic body of the checkered flag

private:
	Car car;
	Player player;				// The players car
	std::vector<AICar*> aiCars; // The vector with the ai car

	CoroutineManager coroutineManager;	// Coroutine

	std::vector<PhysBody*> carsPhys;	// Vector physic bodies of the cars for the initial menu (car selecition)
	std::string playerIdSelected;		// The id of the player

	std::vector<Car*> allCars;			// Vector of all the cars

	PhysBody* leftArrowLap;
	PhysBody* righttArrowLap;


	float timeToNextState = 0.0f;		// Time to pass to the next state

	// Cars physics bodies
	PhysBody* pAMR23 = NULL;
	PhysBody* pR25 = NULL;
	PhysBody* pGP2Engine = NULL;
	PhysBody* pMp4 = NULL;
	PhysBody* pMp22 = NULL;
	PhysBody* pPinkMerc = NULL;
	PhysBody* pW11 = NULL;
	PhysBody* pRB21 = NULL;

	// Cars textures
	Texture2D tAMR23;
	Texture2D tR25;
	Texture2D tGP2Engine;
	Texture2D tMp4;
	Texture2D tMp22;
	Texture2D tPinkMerc;
	Texture2D tW11;
	Texture2D tRB21;

	// Physics bodies of the checkpoints
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

	Texture2D track;	// Texture of the track

	bool initialMenuStart = false;	// Declare if did the start of initial menu
	bool gamePlayStart = false;		// Declare if did the start of the gameplay state 
	bool lightsOut = false;			// Declare if the race has started
	
	float lightTimer = 0.0f;		// Set the time for starting the race

	int showLap = 1;				// The lap to show

	Camera2D camera = { 0 };		// The camera
};