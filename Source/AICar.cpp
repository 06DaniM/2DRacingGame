#include "AICar.h"
#include "Globals.h"

AICar::AICar() {}
AICar::~AICar() {}

void AICar::Start(Vector2 spawnPoint)
{
    Car::Start(spawnPoint);
    LOG("AI Car Start");
}

void AICar::Update(float dt)
{
    // === AI ===
}

void AICar::CleanUp()
{
    LOG("Cleaning up AI Car");
}

void AICar::Draw()
{
    DrawRectangle(position.x, position.y, width, height, RED);
}