#include "AICar.h"
#include "Globals.h"
#include "Application.h"

AICar::AICar() {}
AICar::~AICar() {}

void AICar::Start(Vector2 spawnPoint)
{
    Car::Start(spawnPoint);
    body = App->physics->CreateRectangle(spawnPoint.x, spawnPoint.y, width, height, false, this, ColliderType::AICAR, DYNAMIC);
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
     body->Draw(width, height, RED);
}

void AICar::OnCollision(PhysBody* physA, PhysBody* physB)
{

}

void AICar::EndCollision(PhysBody* physA, PhysBody* physB)
{

}
