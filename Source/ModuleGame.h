#pragma once

#include "Globals.h"
#include "Module.h"
#include "Car.h"
#include "Player.h"
#include "AICar.h"
#include "Listener.h"
#include "Colliders.h"
#include "Obstacles.h"

#include "p2Point.h"
#include "Coroutine.h"

#include "raylib.h"
#include <vector>
#include <memory>

class PhysBody;
class PhysicEntity;

enum GameState
{
	Opening,
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

	void AssignAICars();
	void CarsDraw();

	void CreateColliders();
	void CreateCheckpoints();
	bool LoadChainFromFile(const char* path, std::vector<int>& outPoints);

	void Destroy(PhysBody* pbody)
	{
		App->physics->DestroyBody(pbody);
	}

public:
	GameState gameState;

	std::vector<std::unique_ptr<Checkpoint>> checkpoints;	// Vector with the checkpoints
	PhysBody* checkeredFlag = NULL;			// Physic body of the checkered flag

private:
	struct CarInfo {
		Texture2D texture;
		std::string id;
		Texture2D imageToShow;
		Sound winSong;
	};

	std::vector<CarInfo> carList;
	int currentCarIndex = 0;

	Car car;
	Player player;				// The players car
	std::vector<AICar*> aiCars; // The vector with the ai car

	Vector2 podiumSlot[3] =
	{
		{706, 500},
		{947, 556},
		{465, 604}
	};

	CoroutineManager coroutineManager;	// Coroutine

	Colliders* trackExt			 = NULL;
	Colliders* trackInt			 = NULL;
	Colliders* trackIntS1		 = NULL;
	Colliders* trackIntS2		 = NULL;
	Colliders* sensorAboveRight  = NULL;
	Colliders* sensorAboveLeft	 = NULL;
	Colliders* sensorBelowUp	 = NULL;
	Colliders* sensorBelowDown	 = NULL;

	std::vector<PhysBody*> uiPhys;			 // Vector physic bodies of the ui for the initial menu
	std::vector<PhysBody*> trackPhys;	     // Vector physic bodies of the track

	std::string playerIdSelected;			 // The id of the player

	std::vector<Car*> allCars;				 // Vector of all the cars

	std::vector<int> externalTrackPoints;	 // External barriers
	std::vector<int> internalTrackPoints;	 // Internal dirt

	std::vector<int> internalTrackPointsS1;	 // Internal barriers of sector 1
	std::vector<int> internalTrackPointsS2;  // Internal barriers of sector 2

	std::vector<int> sensorTrackPointsAboveRight; // Sensor barriers track on top right
	std::vector<int> sensorTrackPointsAboveLeft;  // Sensor barriers track on top left

	std::vector<int> sensorTrackPointsBelowUp;	  // Sensor barriers track on bottom up
	std::vector<int> sensorTrackPointsBelowDown;  // Sensor barriers track on bottom down

	PhysBody* sensorAbove = NULL;
	PhysBody* sensorBelow = NULL;

	PhysBody* leftArrowLap = NULL;		
	PhysBody* righttArrowLap = NULL;

	PhysBody* menuCar = NULL;

	PhysBody* leftArrowCar = NULL;
	PhysBody* rightArrowCar = NULL;


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

	Texture2D openingScreen;
	Texture2D initialMenuScreen;
	Texture2D endScreen;

	Texture2D amr23Stats;
	Texture2D r25Stats;
	Texture2D w11Stats;
	Texture2D gp2Stats;
	Texture2D pinkMercStats;
	Texture2D mc4Stats;
	Texture2D mc22Stats;
	Texture2D rb21Stats;

	Texture2D leftArrow;
	Texture2D rightArrow;

	Texture2D track;	// Texture of the track
	Texture2D leaderBoard;

	//Obstacles
	Texture2D TexCone;

	Music coconutMall;
	Sound f1anthem;
	Sound amr23Win;
	Sound r25Win;
	Sound rb21Win;
	Sound rp20Win;
	Sound mc33Win;
	Sound w11Win;
	Sound sf75Win;
	Sound lMcQueenWin;

	bool initialMenuStart = false;	// Declare if did the start of initial menu
	bool gamePlayStart = false;		// Declare if did the start of the gameplay state 
	bool lightsOut = false;			// Declare if the race has started
	bool f1anthemPlayed = false;
	bool winSoundPlayed = false;
	
	float lightTimer = 0.0f;		// Set the time for starting the race

	int showLap = 1;				// The lap to show

	Camera2D camera = { 0 };		// The camera

	ObstaclesManager obstaclesManager;
};