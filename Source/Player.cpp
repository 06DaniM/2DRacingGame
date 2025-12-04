#include "Player.h"
#include "Globals.h"
#include "Application.h"

#define CHASSIS_INDEX 0
#define WHEEL_FL_INDEX 1
#define WHEEL_FR_INDEX 2
#define WHEEL_BL_INDEX 3
#define WHEEL_BR_INDEX 4

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

    const float maxMotorForce = 20.0f;  // Acceleration
    const float maxSteerAngle = 0.25f;    // Steer angle
    const float lateralGripFactor = 1.0f; // Grip

    // Movement
    float motor = 0.0f;
    float steer = 0.0f;

    // Keys
    if (IsKeyDown(KEY_W)) motor += 1.0f;
    if (IsKeyDown(KEY_S)) motor -= 1.0f;

    if (IsKeyDown(KEY_A)) steer = -1.0f;
    if (IsKeyDown(KEY_D)) steer = 1.0f;

    //Access to the parts of the car
    if (this->parts.size() < 5) return;

    PhysBody* chassis = this->parts[CHASSIS_INDEX];

    PhysBody* wheelFR = this->parts[WHEEL_FL_INDEX];  // FL current → FR real
    PhysBody* wheelBR = this->parts[WHEEL_FR_INDEX];  // FR current → BR real
    PhysBody* wheelFL = this->parts[WHEEL_BL_INDEX];  // BL current → FL real
    PhysBody* wheelBL = this->parts[WHEEL_BR_INDEX];  // BR current → BL real

    // Vector of the wheels
    std::vector<PhysBody*> allWheels = { wheelFL, wheelFR, wheelBL, wheelBR };

    // Apply the forces
    ApplyCarForces(chassis, allWheels, motor, steer, maxMotorForce, maxSteerAngle, lateralGripFactor);

    // Angular damping
    chassis->body->SetAngularDamping(2.0f);
}

void Player::ApplyCarForces(PhysBody* chassis, std::vector<PhysBody*>& wheels, float motor, float steer, float maxMotorForce, float maxSteerAngle, float lateralGripFactor)
{
    if (wheels.size() < 4) return;

    // Obtain the references
    PhysBody* wheelFL = wheels[0];
    PhysBody* wheelFR = wheels[1];

    float steerAngle = steer * maxSteerAngle;

    // Go across all the wheels
    for (PhysBody* wheel : wheels)
    {
        b2Body* wheelBody = wheel->body;

        // Set if is rear or front wheel
        bool isFrontWheel = (wheel == wheelFL || wheel == wheelFR);
        bool isRearWheel = !isFrontWheel;

        b2Vec2 forwardVector;

        // If front wheel -> direction
        if (isFrontWheel)
        {
            float chassisAngle = chassis->body->GetAngle();
            float targetAngle = chassisAngle + steerAngle;

            wheelBody->SetTransform(wheelBody->GetPosition(), targetAngle);

            forwardVector = wheelBody->GetWorldVector(b2Vec2(-1, 0));
        }

        // If rear wheel -> traction
        else
        {
            forwardVector = chassis->body->GetWorldVector(b2Vec2(-1, 0));
        }

        b2Vec2 velocity = wheelBody->GetLinearVelocity();

        // Apply lateral friction
        b2Vec2 lateralVector = b2Cross(1.0f, forwardVector);
        float lateralSpeed = b2Dot(velocity, lateralVector);
        b2Vec2 lateralVelocity = lateralSpeed * lateralVector;

        // Calculation of the impulse friction
        b2Vec2 desiredLateralVelocity = -lateralGripFactor * 0.25f * lateralVelocity;

        b2Vec2 velocityChange = desiredLateralVelocity - lateralVelocity;
        b2Vec2 lateralImpulse = wheelBody->GetMass() * velocityChange;

        // Apply the impulse
        wheelBody->ApplyLinearImpulse(lateralImpulse, wheelBody->GetWorldCenter(), true);


        // Apply traction
        if (isRearWheel)
        {
            float desiredSpeed = motor * maxMotorForce;
            float currentSpeed = b2Dot(velocity, forwardVector);
            float currentForce = (desiredSpeed - currentSpeed);

            // Limit the force
            currentForce = b2Clamp(currentForce, -maxMotorForce, maxMotorForce);

            b2Vec2 motorForce = currentForce * forwardVector;

            wheelBody->ApplyForce(motorForce, wheelBody->GetWorldCenter(), true);
        }
    }
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

    // === Draw the wheels for debug ===
    //if (parts.size() >= 5)
    //{
    //    // Obtén cada rueda
    //    PhysBody* wheelFL = parts[WHEEL_FL_INDEX];
    //    PhysBody* wheelFR = parts[WHEEL_FR_INDEX];
    //    PhysBody* wheelBL = parts[WHEEL_BL_INDEX];
    //    PhysBody* wheelBR = parts[WHEEL_BR_INDEX];

    //    // Posiciones
    //    int fx, fy, rx, ry;

    //    // Front Left
    //    wheelFL->GetPosition(fx, fy);
    //    DrawCircle(fx, fy, 6, RED);

    //    // Front Right
    //    wheelFR->GetPosition(fx, fy);
    //    DrawCircle(fx, fy, 6, GREEN);

    //    // Back Left
    //    wheelBL->GetPosition(rx, ry);
    //    DrawCircle(rx, ry, 6, BLUE);

    //    // Back Right
    //    wheelBR->GetPosition(rx, ry);
    //    DrawCircle(rx, ry, 6, YELLOW);
    //}
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