#pragma once
#include "Globals.h"
#include "p2Point.h"
#include "Application.h"
#include "ModulePhysics.h"

class Car
{
public:
    Car();
    virtual ~Car();

    virtual void Start();
    virtual void Update(float dt);
    virtual void CleanUp();
    virtual void Draw();
};
