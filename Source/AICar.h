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
};