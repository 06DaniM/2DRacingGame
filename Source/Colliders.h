#pragma once
#include "Globals.h"
#include "p2Point.h"
#include "Application.h"
#include "ModulePhysics.h"
#include "ModuleRender.h"
#include "Listener.h"

class Colliders : public Listener
{
public:
    Colliders(float x, float y, int* points, int size, ColliderType type, Listener* listener)
    {
        body = App->physics->CreateChain(
            x, y,
            points, 
            size,
            false,
            this,
            type,
            STATIC
        );
    }

    ~Colliders() {}

    PhysBody* body = NULL;
};