#include "Car.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleGame.h"

#define CHASSIS_INDEX 0
#define WHEEL_FL_INDEX 1
#define WHEEL_FR_INDEX 2
#define WHEEL_BL_INDEX 3
#define WHEEL_BR_INDEX 4

Car::Car() {}

Car::~Car() {}

void Car::Start(Vector2 spawnPoint)
{
    position = spawnPoint;
    pbody = App->physics->CreateCar(spawnPoint.x, spawnPoint.y, width, height, 6, parts, joints);

    PhysBody* chassis = this->parts[CHASSIS_INDEX];

    PhysBody* wheelFR = this->parts[WHEEL_FL_INDEX];  // FL current → FR real
    PhysBody* wheelBR = this->parts[WHEEL_FR_INDEX];  // FR current → BR real
    PhysBody* wheelFL = this->parts[WHEEL_BL_INDEX];  // BL current → FL real
    PhysBody* wheelBL = this->parts[WHEEL_BR_INDEX];  // BR current → BL real

    for (PhysBody* part : parts)
        part->listener = this;

    // Start angle
    float startAngle = -150.0f;
    float startAngleRad = startAngle * DEG2RAD;

    b2Vec2 pos = chassis->body->GetPosition();
    chassis->body->SetTransform(pos, startAngleRad);

    baseMaxSpeed = maxSpeed;
    baseAccelRate = accelRate;

    lap = 0;
    checkpoint = 0;

    LOG("Car Start");
}

void Car::Update(float dt)
{
    float carX, carY, cpX, cpY;
    GetCarAndCheckPos(carX, carY, cpX, cpY);

    // Distance to the next checkpoint
    distanceToNextCheckpoint = b2Distance(
        b2Vec2(carX, carY),
        b2Vec2(cpX, cpY)
    );

    UpdateAbility(dt);
}

void Car::ApplyPhysic()
{
    float dt = GetFrameTime();

    velocity += targetAccel * dt;

    float maxSp = maxSpeed;

    if (inDirt) maxSp = 5;

    if (velocity > maxSp) velocity = maxSp;
    if (velocity < -maxSp * 0.4f) velocity = -maxSp * 0.4f;

    // Normalize speed between 0 - 1
    float speed01 = fabs(velocity) / baseMaxSpeed;
    if (speed01 > 0.8f) speed01 = 0.8f;

    // Faster it goes less it turns
    float steerFactor = 1.0f - speed01;

    // Final steer angle
    float currentSteer = maxSteerAngle * steerFactor;

    // Access to the parts of the car
    if (parts.size() < 5) return;

    chassis = parts[CHASSIS_INDEX];

    wheelFR = parts[WHEEL_FL_INDEX];
    wheelBR = parts[WHEEL_FR_INDEX];
    wheelFL = parts[WHEEL_BL_INDEX];
    wheelBL = parts[WHEEL_BR_INDEX];

    std::vector<PhysBody*> allWheels = { wheelFL, wheelFR, wheelBL, wheelBR };

    float motorForceNormalized = velocity / baseMaxSpeed;

    ApplyCarForces(
        chassis,
        allWheels,
        motorForceNormalized,
        steer,
        maxMotorForce,
        currentSteer,
        lateralGripFactor
    );

    chassis->body->SetAngularDamping(2.0f);
}

void Car::ApplyCarForces(PhysBody* chassis, std::vector<PhysBody*>& wheels, float motor, float steer, float maxMotorForce, float maxSteerAngle, float lateralGripFactor)
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
            forwardVector = chassis->body->GetWorldVector(b2Vec2(-1, 0));

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

void Car::GetCarAndCheckPos(float& carX, float& carY, float& cpX, float& cpY) const
{
    int cx, cy, cpx, cpy;
    pbody->GetPosition(cx, cy);
    carX = (float)cx;
    carY = (float)cy;

    if (checkpoint < App->scene_intro->checkpoints.size())
        App->scene_intro->checkpoints[checkpoint]->body->GetPosition(cpx, cpy);
    else
        App->scene_intro->checkeredFlag->GetPosition(cpx, cpy);

    cpX = (float)cpx;
    cpY = (float)cpy;
}

void Car::ActivateAbility()
{
    doingAbility = true;
    canAbility = false;
    abilityTimer = 0.0f;

    maxSpeed = 30;
    accelRate = baseAccelRate * 2;
}

void Car::UpdateAbility(float dt)
{
    if (!doingAbility) return;

    abilityTimer += dt;

    if (abilityTimer >= 4.0f)
    {
        EndAbility();
    }
}

void Car::EndAbility()
{
    doingAbility = false;

    maxSpeed = baseMaxSpeed;
    accelRate = baseAccelRate;
}

void Car::CleanUp()
{
    LOG("Cleaning up Car");
    UnloadTexture(texture);
    for (PhysBody* part : parts)
    {
        App->physics->DestroyBody(part);
        part = NULL;
    }
}

void Car::Draw() 
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

void Car::Destroy()
{
    for (auto j : joints)
        App->physics->DestroyJoint(j);
    joints.clear();

    for (auto p : parts)
        App->physics->DestroyBody(p);
    parts.clear();
}