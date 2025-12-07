#include "AICar.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleGame.h"

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
    if (!pbody) return;

    // AI
    CalculateMove();

    // Move
    MoveAI();

    return Car::Update(dt);
}

void AICar::CalculateMove()
{
    float carX, carY, cpX, cpY;
    GetCarAndCheckPos(carX, carY, cpX, cpY);

    // Calculate the distance to the checkpoint
    float dx = carX - cpX;
    float dy = carY - cpY;

    // Convert to agnle
    float desiredAngle = atan2f(dy, dx);
    float currentAngle = pbody->body->GetAngle();

    // Angle diff
    float angleDiff = desiredAngle - currentAngle;
    while (angleDiff > 3.14159f) angleDiff -= 6.28318f;
    while (angleDiff < -3.14159f) angleDiff += 6.28318f;

    // Convert angle diff to the steer
    float steer = 0.0f;
    if (angleDiff > 0.1f) steer = 1.0f;
    else if (angleDiff < -0.1f) steer = -1.0f;
    else steer = 0.0f;

    // Automatic acceleration
    float targetAccel = accelRate;

    // Save the values
    this->aiSteer = steer;
    this->aiAccel = targetAccel;
}

void AICar::MoveAI()
{
    if (!canMove) return;

    float dt = GetFrameTime();
    steer = aiSteer;
    targetAccel = aiAccel;

    ApplyPhysic();
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
