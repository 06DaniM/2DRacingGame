#include "Obstacles.h"
#include "Application.h"
#include "ModulePhysics.h"
#include "Globals.h"

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

    // Fallback
    DrawRectangle((int)(x - width * 0.5f), (int)(y - height * 0.5f), (int)width, (int)height, RED);
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
        fixture->SetRestitution(0.05f); //rebote
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
