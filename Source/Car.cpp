#include "Car.h"
#include "Globals.h"

Car::Car() {}

Car::~Car() {}

void Car::Start(Vector2 spawnPoint)
{
    position = spawnPoint;
    LOG("Car Start");
}

void Car::Update(float dt)
{
}

void Car::CleanUp()
{
    LOG("Cleaning up Car");
}

void Car::Draw() {}