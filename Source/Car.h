#pragma once
#include "Globals.h"
#include "p2Point.h"
#include "Application.h"
#include "ModulePhysics.h"
#include "ModuleRender.h"
#include "Listener.h"

class Checkpoint
{
public:
    Checkpoint(float x, float y, float w, float h, int index, float angle, Listener* listener) : width(w), height(h), index(index)
    {
        body = App->physics->CreateRectangle(x, y, width, height, angle, true, listener, ColliderType::CHECKPOINT, STATIC);
        body->n = index;
        position = { x,y };
    }

    ~Checkpoint()
    {
        if (body) App->physics->DestroyBody(body);
    }

    b2Vec2 GetPosition() { return position; }

    b2Vec2 position = { 0,0 };

    PhysBody* body = NULL;
    float width = 0.0f;
    float height = 0.0f;
    int index;
};

class Car
{
public:
    Car();
    virtual ~Car();

    virtual void Start(Vector2 spawnPoint);
    virtual void Update(float dt);
    virtual void CleanUp();
    virtual void Draw();

    virtual void OnCollision(PhysBody* physA, PhysBody* physB) {}
    virtual void EndCollision(PhysBody* physA, PhysBody* physB) {}

    void Destroy();

protected:

    int texW = 73;
    int texH = 29;

    float width = 50;
    float height = 25;

    float acceleration = 15.0f;
    float maxSpeed = 10.0f;
    float turnSpeed = 1.0f;
    float brakeForce = 8.0f;

    int checkPoint = 0;

public:
    std::vector<PhysBody*> parts;
    std::vector<b2Joint*> joints;

    Vector2 position;
    PhysBody* pbody = nullptr;

    Texture2D texture;

    bool canMove = true;

    int totalLaps = 3;
    int lap = 1;

    int racePosition = -1;
    int checkpoint = 0;
    float distanceToNextCheckpoint = 0.0f;

    float fastestLapTime = 0.0f;
    float previousLapTime = 0.0f;
    float currentLapTime = 0.0f;
};
