#include "AICar.h"
#include "Globals.h"
#include "Application.h"

AICar::AICar() {}
AICar::~AICar() {}

void AICar::Start(Vector2 spawnPoint)
{
    Car::Start(spawnPoint);
    pbody->ctype = ColliderType::AICAR;
    pbody->listener = this;

    LOG("AI Car Start");
}

void AICar::Update(float dt)
{
    // === AI ===

    return Car::Update(dt);
}

void AICar::CleanUp()
{
    LOG("Cleaning up AI Car");
    return Car::CleanUp();
}

void AICar::Draw()
{
    int x, y;
    pbody->GetPosition(x, y);

    float rotation = pbody->body->GetAngle() * RAD2DEG;

    DrawTextureEx(texture, { (float)x - texW / 2, (float)y - texH / 2 }, rotation, 1.0f, WHITE);
}

void AICar::OnCollision(PhysBody* physA, PhysBody* physB)
{

}

void AICar::EndCollision(PhysBody* physA, PhysBody* physB)
{

}
