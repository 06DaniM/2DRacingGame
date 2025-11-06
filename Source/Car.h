#pragma once
#include "Globals.h"
#include "p2Point.h"
#include "ModulePhysics.h"

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

public:
    PhysBody* body = nullptr;
    Vector2 position;
    float width = 40.0f;
    float height = 20.0f;
};
