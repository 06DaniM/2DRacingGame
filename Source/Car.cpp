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
    if (IsKeyPressed(KEY_F2))
        canMove = !canMove;

    Checkpoint* cp = App->scene_intro->checkpoints[checkpoint];

    b2Vec2 cpPos = cp->GetPosition();
    int x, y;
    pbody->GetPosition(x, y);
    b2Vec2 pos = { (float)x, (float)y };

    distanceToNextCheckpoint = b2Distance(pos, cpPos);
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