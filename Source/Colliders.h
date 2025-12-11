#pragma once
#include "Globals.h"
#include "p2Point.h"
#include "Application.h"
#include "ModulePhysics.h"
#include "ModuleRender.h"
#include "Listener.h"
#include "PhysicCategory.h"

class Colliders : public Listener
{
public:
    Colliders(float x, float y, int* points, int size, ColliderType type, Listener* listener, std::vector<PhysBody*>& trackPhys, uint16 categoryBits = PhysicCategory::DEFAULT, uint16 maskBits = 0xFFFF)
    {
        body = App->physics->CreateChain(
            x, y,
            points, 
            size,
            false,
            this,
            type,
            STATIC,
            categoryBits,
            maskBits
        );

        trackPhys.push_back(body);
    }

    ~Colliders() {}

    PhysBody* body = NULL;
};