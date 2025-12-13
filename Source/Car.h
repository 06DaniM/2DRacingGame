#pragma once
#include "Globals.h"
#include "p2Point.h"
#include "Application.h"
#include "ModulePhysics.h"
#include "ModuleRender.h"
#include "Listener.h"
#include "PhysicCategory.h"

class Checkpoint
{
public:
    Checkpoint(float x, float y, float w, float h, int index, float angle, Listener* listener) : 
        width(w), height(h), index(index)
    {
        body = App->physics->CreateRectangle(x, y, width, height, angle, true, listener, ColliderType::CHECKPOINT, STATIC);
        body->n = index;
        position = { x,y };
    }

    ~Checkpoint() {}

    b2Vec2 GetPosition() const { return position; }

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
    void ApplyPhysic();
    void ApplyCarForces(PhysBody* chassis, std::vector<PhysBody*>& wheels, float motor, float steer, float maxMotorForce, float maxSteerAngle, float lateralGripFactor);
    void GetCarAndCheckPos(float& carX, float& carY, float& cpX, float& cpY) const;

    void ActivateAbility();
    void UpdateAbility(float dt);
    void EndAbility();

protected:

    int texW = 73;
    int texH = 29;

    float width = 50;
    float height = 25;

    float motor = 0.0f;            // Current motor force

    float velocity = 0.0f;         // Current velocity
    float maxSpeed = 25.0f;        // Maximum speed
    float accelRate = 18.0f;       // Acceleration

    float baseMaxSpeed = 0.0f;
    float baseAccelRate = 0.0f;

    const float brakeRate = 25.0f; // Break force
    const float drag = 15.0f;      // Drag

    const float maxMotorForce = 20.0f;      // Max force
    const float maxSteerAngle = 0.4f;       // Max steering
    const float lateralGripFactor = 1.0f;   // Grip

    bool doingAbility = false;
    float abilityTimer = 0.0f;

    PhysBody* chassis = NULL;

    PhysBody* wheelFR = NULL;  // FL current → FR real
    PhysBody* wheelBR = NULL;  // FR current → BR real
    PhysBody* wheelFL = NULL;  // BL current → FL real
    PhysBody* wheelBL = NULL;  // BR current → BL real

public:
    std::vector<PhysBody*> parts;
    std::vector<b2RevoluteJoint*> joints;

    Vector2 position;
    PhysBody* pbody = nullptr;

    Texture2D texture;
    Sound winSong;

    bool canMove = true;

    int totalLaps = 3;
    int lap = 1;

    int racePosition = -1;
    int checkpoint = 0;
    float distanceToNextCheckpoint = 0.0f;

    float fastestLapTime = 0.0f;
    float previousLapTime = 0.0f;
    float currentLapTime = 0.0f;

    bool inDirt = false;

    float steer = 0.0f;
    float targetAccel = 0.0f;

    bool canAbility = false;
    int dirtContacts = 0;
};