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

    // Rotation in angles
    float rotation = pbody->body->GetAngle() * RAD2DEG;

    Rectangle sourceRec = { 0.0f, 0.0f, (float)texW, (float)texH };
    Rectangle destRec = { (float)x, (float)y, (float)texW, (float)texH };
    Vector2 origin = { texW / 2.0f, texH / 2.0f };

    DrawTexturePro(texture, sourceRec, destRec, origin, rotation, WHITE);
}

void AICar::OnCollision(PhysBody* physA, PhysBody* physB)
{

}

void AICar::EndCollision(PhysBody* physA, PhysBody* physB)
{

}
