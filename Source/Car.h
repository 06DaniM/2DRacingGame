#pragma once
#include "Globals.h"
#include "p2Point.h"
#include "Application.h"
#include "ModulePhysics.h"
#include "ModuleRender.h"

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

    bool canMove = true;

    int checkPoint = 0;

public:
    std::vector<PhysBody*> parts;

    Vector2 position;
    PhysBody* pbody = nullptr;

    Texture2D texture;

    int totalLaps = 3;
    int lap = 0;
    int checkpoint = 0;

    float fastestLapTime = 0.0f;
    float previousLapTime = 0.0f;
    float currentLapTime = 0.0f;
};
