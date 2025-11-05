#include "Player.h"
#include "Globals.h"

Player::Player() {}
Player::~Player() {}

void Player::Start()
{
    Car::Start();
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
    DrawRectangle(SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2, 20, 20, GREEN);
}