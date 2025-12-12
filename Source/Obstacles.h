#pragma once

#include "Globals.h"
#include "Listener.h"
#include <vector>

class Obstacle : public Listener
{
public:
    Obstacle() = default;
    virtual ~Obstacle() = default;

    virtual void Start(const Vector2& spawnPoint);
    virtual void Update(float dt);
    virtual void CleanUp();
    virtual void Draw();

    virtual void OnCollision(PhysBody* physA, PhysBody* physB) override {}
    virtual void EndCollision(PhysBody* physA, PhysBody* physB) override {}

    // Permitir asignar textura desde el exterior (ModuleGame / Manager)
    void SetTexture(const Texture2D& tex) { texture = tex; }



protected:
    PhysBody* body = nullptr;
    Vector2 position = { 0, 0 };

    Texture2D texture = { 0 };
    float width = 0.0f;
    float height = 0.0f;
    float radius = 0.0f;
};

// Manager para crear/almacenar/gestionar obstaculos desde una lista de posiciones
class ObstaclesManager
{
public:
    ObstaclesManager() = default;
    ~ObstaclesManager();

    void Start() {}
    void Update(float dt);
    void Draw();
    void CleanUp();

    
    void SetConeTexture(const Texture2D& tex) { coneTexture = tex; }

    // Spawn utilities
    Obstacle* SpawnCone(const Vector2& pos);
    void SpawnFromList(const std::vector<Vector2>& positions);
    void Clear();

private:
    std::vector<Obstacle*> obstacles;
    Texture2D coneTexture = { 0 };
};

//Obstaculo que si te das un golpe con el te relentiza del golpe y sale disparado (como un cono)
class Cone : public Obstacle
{
public:
    Cone();

    void Start(const Vector2& spawnPoint) override;

    void OnCollision(PhysBody* physA, PhysBody* physB) override;
    void EndCollision(PhysBody* physA, PhysBody* physB) override;
};