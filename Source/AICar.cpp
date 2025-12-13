#include "AICar.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleGame.h"

AICar::AICar() {}
AICar::~AICar() {}

void AICar::Start(Vector2 spawnPoint)
{
    Car::Start(spawnPoint);

    pbody->listener = this;

    fastestLapTime = 0.0f;
    currentLapTime = 0.0f;

    canMove = false;

    LOG("AI Car Start");
}

void AICar::Update(float dt)
{
    if (!pbody) return;

    // AI
    CalculateMove();

    // Move
    MoveAI();

    if (checkpoint == 25 && canRandomize)
    {
        if (GetRandomValue(0, 2) == 0)
            ActivateAbility();
        canRandomize = false;
    }

    else if (checkpoint == 34 && canAbility)
        ActivateAbility();

    return Car::Update(dt);
}

void AICar::CalculateMove()
{
    float carX, carY, cpX, cpY;
    GetCarAndCheckPos(carX, carY, cpX, cpY);

    // Calculate the distance to the checkpoint
    float dx = carX - cpX;
    float dy = carY - cpY;
    float dist = sqrtf(dx * dx + dy * dy);

    // Get the angle
    float desiredAngle = atan2f(dy, dx);
    float currentAngle = pbody->body->GetAngle();

    // Angle diff
    float angleDiff = desiredAngle - currentAngle;

    while (angleDiff > 3.14159f) angleDiff -= 6.28318f;
    while (angleDiff < -3.14159f) angleDiff += 6.28318f;

    // Steering
    float steer = 0.0f;
    if (angleDiff > 0.12f) steer = 1.0f;
    else if (angleDiff < -0.12f) steer = -1.0f;

    // Acceleration
    float targetAccel = accelRate;

    // Breaking
    if (fabs(angleDiff) > 0.15f && velocity > 5)
        targetAccel = -brakeRate;

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

void AICar::Draw() { return Car::Draw(); }

void AICar::OnCollision(PhysBody* physA, PhysBody* physB)
{

}

void AICar::EndCollision(PhysBody* physA, PhysBody* physB)
{

}
