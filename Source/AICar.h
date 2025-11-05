#pragma once
#include "Car.h"

class AICar : public Car
{
public:
    AICar();
    virtual ~AICar();

    void Start() override;
    void Update(float dt) override;
    void CleanUp() override;
    void Draw() override;
};