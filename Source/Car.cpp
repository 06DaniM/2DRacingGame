#include "Car.h"
#include "Globals.h"
#include "Application.h"

Car::Car() {}

Car::~Car() {}

void Car::Start(Vector2 spawnPoint)
{
    position = spawnPoint;
    pbody = App->physics->CreateCar(spawnPoint.x, spawnPoint.y, width, height, 6, parts, joints);
    LOG("Car Start");
}

void Car::Update(float dt)
{
    int x, y;
    pbody->GetPosition(x, y);
    position = { (float)x, (float)y };

    if (IsKeyPressed(KEY_F2))
        canMove = !canMove;

    //distanceToNextCheckpoint = b2Distance(position, checkPoints[checkPoint].)
}

void Car::CleanUp()
{
    LOG("Cleaning up Car");

    pbody = NULL;
    UnloadTexture(texture);
}

void Car::Draw() {}

void Car::Destroy()
{
    for (auto j : joints)
        App->physics->DestroyJoint(j);
    joints.clear();

    for (auto p : parts)
        App->physics->DestroyBody(p);
    parts.clear();
}