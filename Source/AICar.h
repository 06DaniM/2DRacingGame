#pragma once
#include "Car.h"
#include "Listener.h"

class AICar : public Car, public Listener
{
public:
    AICar();
    virtual ~AICar();

    void Start(Vector2 spawnPoint) override;
    void Update(float dt) override;
    void CleanUp() override;
    void Draw() override;

    void OnCollision(PhysBody* physA, PhysBody* physB) override;
    void EndCollision(PhysBody* physA, PhysBody* physB) override;

private:
    void CalculateMove();
    void MoveAI();

public:

private:
    float aiSteer = 0.0f;
    float aiAccel = 0.0f;

    float checkpointOffset = 0.0f;  // Horizontal offset
    int lastCheckpoint = -1;        // To know if needs a new point

};