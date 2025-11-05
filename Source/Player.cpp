#include "Player.h"
#include "Globals.h"

Player::Player() {}
Player::~Player() {}

void Player::Start(Vector2 spawnPoint)
{
    Car::Start(spawnPoint);
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
    DrawRectangle(position.x, position.y, width, height, GREEN);
}