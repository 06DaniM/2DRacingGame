#include "Player.h"
#include "Globals.h"
#include "Application.h"

Player::Player() {}
Player::~Player() {}

void Player::Start(Vector2 spawnPoint)
{
    Car::Start(spawnPoint);

    body = App->physics->CreateRectangle(spawnPoint.x, spawnPoint.y, width, height, false, this, ColliderType::PLAYER, DYNAMIC);
    LOG("Player Start");
}

void Player::Update(float dt)
{
    // === Input and movement ===
}

void Player::CleanUp()
{
    LOG("Cleaning up Player");
}

void Player::Draw()
{
    body->Draw(width, height, GREEN);
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