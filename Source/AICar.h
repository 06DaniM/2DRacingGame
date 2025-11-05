#pragma once
#include "Car.h"

class AICar : public Car
{
public:
    AICar();
    virtual ~AICar();

    void Start(Vector2 spawnPoint) override;
    void Update(float dt) override;
    void CleanUp() override;
    void Draw() override;
};