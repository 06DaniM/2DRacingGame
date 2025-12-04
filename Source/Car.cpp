#include "Car.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleGame.h"

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
    // Obtain checkpoint or checkered flag
    int cpX, cpY;
    if (checkpoint < App->scene_intro->checkpoints.size())
        App->scene_intro->checkpoints[checkpoint]->body->GetPosition(cpX, cpY);
    else
        App->scene_intro->checkeredFlag->GetPosition(cpX, cpY);

    // Car position
    int carX, carY;
    pbody->GetPosition(carX, carY);

    // Distance to the next checkpoint
    distanceToNextCheckpoint = b2Distance(
        b2Vec2((float)carX, (float)carY),
        b2Vec2((float)cpX, (float)cpY)
    );
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