#pragma once
#include "Car.h"

class Player : public Car
{
public:
    Player();
    virtual ~Player();

    void Start() override;
    void Update(float dt) override;
    void CleanUp() override;
    void Draw() override;
};