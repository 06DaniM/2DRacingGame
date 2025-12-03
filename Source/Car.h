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
    Vector2 position;
    PhysBody* pbody = nullptr;

    Texture2D texture;
    int lap = 0;
};
