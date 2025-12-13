#pragma once
#include "Car.h"
#include "Listener.h"

class Player : public Car, public Listener
{
public:
    Player();
    virtual ~Player();

    void Start(Vector2 spawnPoint) override;
    void Update(float dt) override;
    void CleanUp() override;
    void Draw() override;

    void OnCollision(PhysBody* physA, PhysBody* physB) override;
    void EndCollision(PhysBody* physA, PhysBody* physB) override;

private:
    void Move();

public:

private:
    Sound engine;
};