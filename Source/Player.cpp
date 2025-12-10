#include "Player.h"
#include "Globals.h"
#include "Application.h"

Player::Player() {}
Player::~Player() {}

void Player::Start(Vector2 spawnPoint)
{
    Car::Start(spawnPoint);
    pbody->ctype = ColliderType::PLAYER;
    pbody->listener = this;

    lap = 0;
    checkPoint = 0;

    previousLapTime = 0.0f;
    fastestLapTime = 0.0f;
    currentLapTime = 0.0f;

    motor = 0;

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

    float dt = GetFrameTime();
    steer = 0.0f;

    targetAccel = 0.0f;

    if (IsKeyDown(KEY_W) || IsKeyDown(KEY_UP))
        targetAccel += accelRate;

    if (IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN))
        targetAccel -= brakeRate;

    if (!IsKeyDown(KEY_W) && !IsKeyDown(KEY_S) && !IsKeyDown(KEY_UP) && !IsKeyDown(KEY_DOWN))
    {
        if (velocity > 0)
            velocity -= drag * dt;
        else if (velocity < 0)
            velocity += drag * dt;

        if (fabs(velocity) < 0.1f)
            velocity = 0;
    }

    if (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))  steer = -1.0f;
    if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) steer = 1.0f;

    ApplyPhysic();
}

void Player::CleanUp()
{
    LOG("Cleaning up Player");
}

void Player::Draw()
{
    if (!pbody) return;

    int x, y;
    pbody->GetPosition(x, y);

    // Rotation in angles
    float rotation = pbody->body->GetAngle() * RAD2DEG;

    Rectangle sourceRec = { 0.0f, 0.0f, (float)texW, (float)texH };
    Rectangle destRec = { (float)x, (float)y, (float)texW, (float)texH };
    Vector2 origin = { texW / 2.0f, texH / 2.0f };

    DrawTexturePro(texture, sourceRec, destRec, origin, rotation, WHITE);
}

void Player::OnCollision(PhysBody* physA, PhysBody* physB) {}
void Player::EndCollision(PhysBody* physA, PhysBody* physB) {}