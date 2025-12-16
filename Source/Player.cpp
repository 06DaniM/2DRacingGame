#include "Player.h"
#include "Globals.h"
#include "Application.h"

Player::Player() {}
Player::~Player() {}

void Player::Start(Vector2 spawnPoint)
{
    Car::Start(spawnPoint);

    previousLapTime = 0.0f;
    fastestLapTime = 0.0f;
    currentLapTime = 0.0f;

    canMove = true; // Cambiar a false

    motor = 0;
    velocity = 0;

    engine = LoadSound("Assets/SFX/F1_Motor.wav");
    SetSoundVolume(engine, 0.5f);

    LOG("Player Start");
}

void Player::Update(float dt)
{
    if (!pbody) return;

    Move();

    if (IsKeyPressed(KEY_SPACE) && canAbility && !doingAbility)
        ActivateAbility();

    // Set the volume of the engine depending on the velocity
    if (velocity > 1.0f)
    {
        if (!IsSoundPlaying(engine))
            PlaySound(engine);

        float t = velocity / baseMaxSpeed;
        if (t > 1.0f) t = 1.0f;

        SetSoundVolume(engine, t * 0.5f);
    }
    else
        StopSound(engine);

    Car::Update(dt);
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

void Player::Draw() { return Car::Draw(); }

void Player::OnCollision(PhysBody* physA, PhysBody* physB) {}
void Player::EndCollision(PhysBody* physA, PhysBody* physB) {}