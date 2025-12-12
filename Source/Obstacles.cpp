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

    if (texture.id != 0)
    {
        Rectangle sourceRec = { 0.0f, 0.0f, (float)texture.width, (float)texture.height };
        Rectangle destRec = { (float)(x - width / 2.0f), (float)(y - height / 2.0f), width, height };
        Vector2 origin = { (float)texture.width / 2.0f, (float)texture.height / 2.0f };

        float rotation = body->GetRotation();
        DrawTexturePro(texture, sourceRec, destRec, origin, rotation, WHITE);
        return;
    }

    DrawRectangle(x - (int)(width / 2.0f), y - (int)(height / 2.0f), (int)width, (int)height, RED);
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

void ObstaclesManager::Clear()
{
    CleanUp();
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

}

void Cone::OnCollision(PhysBody* physA, PhysBody* physB)
{
    switch (physB->ctype) {
    case ColliderType::CAR:
        LOG("Collided with Car");
        break;
    }
}

void Cone::EndCollision(PhysBody* physA, PhysBody* physB)
{
    //para que pollas sirve esto?!!
}
