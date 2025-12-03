#include "Car.h"
#include "Globals.h"
#include "Application.h"

Car::Car() {}

Car::~Car() {}

void Car::Start(Vector2 spawnPoint)
{
    position = spawnPoint;
    pbody = App->physics->CreateCar(spawnPoint.x, spawnPoint.y, width, height, 6);
    LOG("Car Start");
}

void Car::Update(float dt)
{
    if (IsKeyPressed(KEY_F2))
        canMove = !canMove;
}

void Car::CleanUp()
{
    LOG("Cleaning up Car");
}

void Car::Draw() {}