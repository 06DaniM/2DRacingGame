#include "Obstacles.h"
#include "Application.h"
#include "ModulePhysics.h"
#include "Globals.h"
#include "ModuleGame.h"
#include "Car.h"
#include <cmath>

// ---------------- Obstacle base ----------------
void Obstacle::Start(const Vector2& spawnPoint)
{
    position = spawnPoint;
}

void Obstacle::Update(float dt)
{
    
}

void Obstacle::CleanUp()
{
    if (body)
    {
        App->physics->DestroyBody(body);
        body = nullptr;
    }
}

void Obstacle::Draw()
{
}

// ---------------- ObstaclesManager ----------------
ObstaclesManager::~ObstaclesManager()
{
    CleanUp();
}

void ObstaclesManager::Update(float dt)
{
    for (auto o : obstacles)
    {
        if (o) o->Update(dt);
    }

    for (auto it = obstacles.begin(); it != obstacles.end(); )
    {
        Obstacle* o = *it;
        if (o && o->IsDead())
        {
            o->CleanUp();
            delete o;
            it = obstacles.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void ObstaclesManager::Draw()
{
    for (auto o : obstacles)
    {
        if (o) o->Draw();
    }
}

void ObstaclesManager::CleanUp()
{
    for (auto o : obstacles)
    {
        if (o)
        {
            o->CleanUp();
            delete o;
        }
    }
    obstacles.clear();
}

Obstacle* ObstaclesManager::SpawnCone(const Vector2& pos)
{
    Cone* cone = new Cone();
    cone->Start(pos);

    if (coneTexture.id != 0)
        cone->SetTexture(coneTexture);
    obstacles.push_back(cone);
    return cone;
}

void ObstaclesManager::SpawnFromList(const std::vector<Vector2>& positions)
{
    for (const auto& p : positions)
        SpawnCone(p);
}

Obstacle* ObstaclesManager::SpawnExplosive(const Vector2& pos)
{
    Explosive* explosive = new Explosive();
    explosive->Start(pos);

    if (explosiveTexture.id != 0)
        explosive->SetTexture(explosiveTexture);

    obstacles.push_back(explosive);
    return explosive;
}

Obstacle* ObstaclesManager::SpawnPuddle(const Vector2& pos)
{
    Puddle* puddle = new Puddle();
    puddle->Start(pos);

    if (puddleTexture.id != 0)
        puddle->SetTexture(puddleTexture);

    obstacles.push_back(puddle);
    return puddle;
}

void ObstaclesManager::SetConeTexture(const Texture& tex)
{
    coneTexture = tex;
}

void ObstaclesManager::SetExplosiveTexture(const Texture& tex)
{
    explosiveTexture = tex;
}

void ObstaclesManager::SetPuddleTexture(const Texture& tex)
{
    puddleTexture = tex;
}

// ---------------- Cone ----------------
Cone::Cone()
{
}

void Cone::Start(const Vector2& spawnPoint)
{
    width = 30.0f;
    height = 30.0f;

    position = spawnPoint;

    body = App->physics->CreateRectangle(
        (int)spawnPoint.x,
        (int)spawnPoint.y,
        (int)width,
        (int)height,
        0.0f,
        false,
        this,
        ColliderType::OBSTACLE,
        DYNAMIC
    );

    if (!body || !body->body) return;

    b2Body* b = body->body;
    b2Fixture* fixture = b->GetFixtureList();

    if (fixture) {
        fixture->SetDensity(3.0f);
        fixture->SetFriction(0.8f);
        fixture->SetRestitution(0.05f);
    }

    b->ResetMassData();

    b->SetLinearDamping(1.5f);
    b->SetAngularDamping(0.8f);
}

void Cone::OnCollision(PhysBody* physA, PhysBody* physB)
{
    switch (physB->ctype) {
    case ColliderType::CAR:
        LOG("Collided with Car");
        physA->body->ApplyLinearImpulseToCenter(1.1f * physB->body->GetLinearVelocity(), true);
        break;
    }
}

void Cone::EndCollision(PhysBody* physA, PhysBody* physB)
{
    switch (physB->ctype) {
    case ColliderType::CAR:
        LOG("Collided with Car");
        break;
    }
}

void Cone::Draw()
{
    if (!body) return;

    int x, y;
    body->GetPosition(x, y);

    float rotation = body->body->GetAngle() * RAD2DEG;

    if (texture.id != 0 && texture.width > 0 && texture.height > 0)
    {
        Rectangle sourceRec = { 0.0f, 0.0f, (float)texture.width, (float)texture.height };
        Rectangle destRec = { (float)x, (float)y, width, height };
        Vector2 origin = { width * 0.5f, height * 0.5f };

        DrawTexturePro(texture, sourceRec, destRec, origin, rotation, WHITE);
        return;
    }

    //DrawRectangle((int)(x - width * 0.5f), (int)(y - height * 0.5f), (int)width, (int)height, RED);
}

// ---------------- Explosive ----------------
Explosive::Explosive()
{
}

void Explosive::Start(const Vector2& spawnPoint)
{
    width = 30.0f;
    height = 30.0f;

    position = spawnPoint;

    body = App->physics->CreateRectangle(
        (int)spawnPoint.x,
        (int)spawnPoint.y,
        (int)width,
        (int)height,
        0.0f,
        false,
        this,
        ColliderType::OBSTACLE,
        DYNAMIC
    );

    if (!body || !body->body) return;

    b2Body* b = body->body;
    b2Fixture* fixture = b->GetFixtureList();

    explosionTexture = LoadTexture("Assets/Textures/Obstacles/Explosion_Sheet.png");

    explosiveAnim = Animator(&explosionTexture, 176, 160);
    explosiveAnim.AddAnim("explosion", 0, 3, 1.0f, false);

    // Valores iniciales de explosi�n
    state = State::Idle;
    explosionRadius = 0.0f;
    explosionElapsed = 0.0f;
    explosionCenter = position;
    affectedCars.clear();
    affectedSet.clear();
    pendingBodyDestroy = false;
    toBeRemoved = false;
}

void Explosive::Update(float dt)
{
    if (state == State::Exploding)
    {
        if (pendingBodyDestroy && body)
        {
            App->physics->DestroyBody(body);
            body = nullptr;
            pendingBodyDestroy = false;
        }
        UpdateExplosion(dt);
    }
}

void Explosive::OnCollision(PhysBody* physA, PhysBody* physB)
{
    switch (physB->ctype) {
    case ColliderType::CAR:
        LOG("Explosive collided with Car");
        TriggerExplosion();
    }
}

void Explosive::EndCollision(PhysBody* physA, PhysBody* physB)
{
}

void Explosive::TriggerExplosion()
{
    if (state != State::Idle) return;

    state = State::Exploding;
    explosionRadius = 1.0f;
    explosionElapsed = 0.0f;
    affectedCars.clear();
    affectedSet.clear();

    int bx, by;
    if (body && body->body)
    {
        body->GetPosition(bx, by);
        explosionCenter.x = (float)bx;
        explosionCenter.y = (float)by;

        // Marcar para destrucci�n en Update
        pendingBodyDestroy = true;
    }
    else
    {
        explosionCenter = position;
        pendingBodyDestroy = false;
    }
}

void Explosive::UpdateExplosion(float dt)
{
    if (state != State::Exploding) return;

    explosionElapsed += dt;
    explosionRadius = 1.0f + explosionSpeed * explosionElapsed;
    if (explosionRadius > explosionMaxRadius) explosionRadius = explosionMaxRadius;

    float explosionRadius_m = PIXELS_TO_METERS(explosionRadius);
    float maxR_m = PIXELS_TO_METERS(explosionMaxRadius);

    float cx_m = PIXELS_TO_METERS(explosionCenter.x);
    float cy_m = PIXELS_TO_METERS(explosionCenter.y);

    const std::vector<Car*>& cars = App->scene_intro->GetAllCars();

    for (Car* car : cars)
    {
        if (!car || !car->pbody || !car->pbody->body) continue;
        if (affectedSet.find(car) != affectedSet.end()) continue;

        b2Vec2 posCar = car->pbody->body->GetPosition();
        float dx = posCar.x - cx_m;
        float dy = posCar.y - cy_m;
        float dist = std::sqrt(dx*dx + dy*dy);

        if (dist <= explosionRadius_m)
        {
            b2Vec2 dir;
            if (dist <= 0.0005f) dir = b2Vec2(0.0f, -1.0f);
            else dir = b2Vec2(dx / dist, dy / dist);

            float factor = 1.0f - (dist / maxR_m);
            if (factor < 0.0f) factor = 0.0f;

            float bodyMass = car->pbody->body->GetMass();

            if (bodyMass <= 0.0f) bodyMass = 1.0f;

            b2Vec2 impulse = explosionForce * bodyMass * factor * dir;

            car->pbody->body->ApplyLinearImpulseToCenter(impulse, true);

            affectedSet.insert(car);
            affectedCars.push_back(car);
        }
    }

    if (explosionRadius >= explosionMaxRadius)
    {
        affectedCars.clear();
        affectedSet.clear();
        state = State::Done;
        toBeRemoved = true;
    }
}

void Explosive::Draw()
{
    if (body)
    {
        int x, y;
        body->GetPosition(x, y);

        float rotation = body->body->GetAngle() * RAD2DEG;

        if (texture.id != 0 && texture.width > 0 && texture.height > 0)
        {
            Rectangle sourceRec = { 0.0f, 0.0f, (float)texture.width, (float)texture.height };
            Rectangle destRec = { (float)x, (float)y, width, height };
            Vector2 origin = { width * 0.5f, height * 0.5f };

            DrawTexturePro(texture, sourceRec, destRec, origin, rotation, WHITE);
        }
        else
        {
            DrawRectangle((int)(x - width * 0.5f), (int)(y - height * 0.5f), (int)width, (int)height, ORANGE);
        }
    }
    else
    {
        DrawRectangle((int)(explosionCenter.x - width * 0.5f), (int)(explosionCenter.y - height * 0.5f), (int)width, (int)height, ORANGE);
    }

    if (state == State::Exploding || explosionRadius > 0.0f)
    {
        float cx = explosionCenter.x;
        float cy = explosionCenter.y;

        explosiveAnim.Play("explosion");
        explosiveAnim.Draw({ cx,cy }, 2);
    }
}

void Explosive::CleanUp()
{
    if (body)
    {
        App->physics->DestroyBody(body);
        body = nullptr;
    }

    affectedCars.clear();
    affectedSet.clear();
    pendingBodyDestroy = false;
    explosionRadius = 0.0f;
    toBeRemoved = true;
}

// ---------------- Puddle ----------------
Puddle::Puddle()
{
}

void Puddle::Start(const Vector2& spawnPoint)
{
    radius = 30.0f;
    position = spawnPoint;
    
    body = App->physics->CreateCircle(
        (int)spawnPoint.x,
        (int)spawnPoint.y,
        (int)radius,
        true,   // SENSOR
        this,
        ColliderType::OBSTACLE,
        STATIC
    );

    toBeRemoved = false;
}

void Puddle::OnCollision(PhysBody* physA, PhysBody* physB)
{
    PhysBody* other = (physA->ctype == ColliderType::OBSTACLE) ? physB : physA;

    if (other->ctype != ColliderType::CAR &&
        other->ctype != ColliderType::WHEEL)
        return;

    Car* car = dynamic_cast<Car*>(other->listener);
    if (!car) return;

    car->SetGripMultiplier(0.15f);
    car->SetPuddleTimer(1.0f);
    car->SetInPuddle(true);
}

void Puddle::Draw()
{
    if (body && body->body)
    {
        int x, y;
        body->GetPosition(x, y);

        DrawTextureEx(texture, { (float)x - texture.width, (float)y - texture.height}, 0.0f, 2, WHITE);
    }
}