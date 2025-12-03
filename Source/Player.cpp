#include "Player.h"
#include "Globals.h"
#include "Application.h"

Player::Player() {}
Player::~Player() {}

void Player::Start(Vector2 spawnPoint)
{
    Car::Start(spawnPoint);

    LOG("Player Start");
}

void Player::Update(float dt)
{
    if (!pbody) return;

    Move();

    return Car::Update(dt);
}

void Player::Move()
{
    if (!canMove) return;

    b2Body* b = pbody->body;
    b2Vec2 velocity = b->GetLinearVelocity();
    float currentSpeed = velocity.Length();

    // Direction of the car
    b2Vec2 forward = b->GetWorldVector(b2Vec2(-1, 0)); // Define the front of the car
    b2Vec2 force(0, 0);

    // === ACCELERATING ===
    if (IsKeyDown(KEY_W))
    {
        if (currentSpeed < maxSpeed)
            force = acceleration * forward;
    }

    // === BRAKING ===
    if (IsKeyDown(KEY_S))
    {
        force = -brakeForce * forward;
    }

    // === APPLY THE FORCE ===
    b->ApplyForceToCenter(force, true);

    // === TURN ===
    float rotation = b->GetAngle();

    if (IsKeyDown(KEY_A)) b->ApplyTorque(-1.0f, true);
    if (IsKeyDown(KEY_D)) b->ApplyTorque(1.0f, true);

    // === DAMPEN ===
    b->SetLinearDamping(0.5f);
    b->SetAngularDamping(2.0f);
}

void Player::CleanUp()
{
    LOG("Cleaning up Player");
}

void Player::Draw()
{
    int x, y;
    pbody->GetPosition(x, y);

    //pbody->Draw(width, height, GREEN);

    float rotation = pbody->body->GetAngle() * RAD2DEG;

    DrawTextureEx(texture, { (float)x - texW / 2, (float)y - texH / 2 }, rotation, 1.0f, WHITE);
}

void Player::OnCollision(PhysBody* physA, PhysBody* physB)
{
    switch (physB->ctype)
    {
    case ColliderType::AICAR:
        if (physA->ctype == ColliderType::PLAYER)
        {
            LOG("Collide with an AI");
        }
        break;

    default:
        break;
    }
}

void Player::EndCollision(PhysBody* physA, PhysBody* physB)
{
    switch (physB->ctype)
    {
    case ColliderType::AICAR:
        if (physA->ctype == ColliderType::PLAYER)
        {
            LOG("End collision with an AI");
        }
        break;

    default:
        break;
    }
}