#pragma once

class PhysBody;

class Listener
{
public:
    virtual void OnCollision(PhysBody* physA, PhysBody* physB) {}
    virtual void EndCollision(PhysBody* physA, PhysBody* physB) {}
};