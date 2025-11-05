#pragma once
#include "Car.h"

class Player : public Car
{
public:
    Player();
    virtual ~Player();

    void Start(Vector2 spawnPoint) override;
    void Update(float dt) override;
    void CleanUp() override;
    void Draw() override;
};