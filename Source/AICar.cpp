#include "AICar.h"
#include "Globals.h"

AICar::AICar() {}
AICar::~AICar() {}

void AICar::Start()
{
    Car::Start();
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
    DrawRectangle(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 20, 20, RED);
}