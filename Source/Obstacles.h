#pragma once
#include <vector>
#include <unordered_set>
#include "Listener.h"
#include "Globals.h"
#include "Animation.h"

struct PhysBody;
class Car;

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

    void SetTexture(Texture tex) { texture = tex; }
    bool IsDead() const { return toBeRemoved; }

protected:
    PhysBody* body = nullptr;
    Vector2 position = { 0.0f, 0.0f };
    Texture texture = { 0 };
    float width = 0.0f;
    float height = 0.0f;
    bool toBeRemoved = false;
};

class ObstaclesManager
{
public:
    ~ObstaclesManager();
    void Update(float dt);
    void Draw();
    void CleanUp();

    Obstacle* SpawnCone(const Vector2& pos);
    void SpawnFromList(const std::vector<Vector2>& positions);
    Obstacle* SpawnExplosive(const Vector2& pos);
    Obstacle* SpawnPuddle(const Vector2& pos);

    // Setters para texturas 
    void SetConeTexture(const Texture& tex);
    void SetExplosiveTexture(const Texture& tex);
    void SetPuddleTexture(const Texture& tex);

    Texture coneTexture = { 0 };
    Texture explosiveTexture = { 0 };
    Texture puddleTexture = { 0 };

private:
    std::vector<Obstacle*> obstacles;
};

class Cone : public Obstacle
{
public:
    Cone();
    void Start(const Vector2& spawnPoint) override;
    void OnCollision(PhysBody* physA, PhysBody* physB) override;
    void EndCollision(PhysBody* physA, PhysBody* physB) override;
    void Draw() override;
};

class Explosive : public Obstacle
{
public:
    Explosive();

    void Start(const Vector2& spawnPoint) override;
    void Update(float dt) override;
    void CleanUp() override;
    void Draw() override;

    void OnCollision(PhysBody* physA, PhysBody* physB) override;
    void EndCollision(PhysBody* physA, PhysBody* physB) override;

    void TriggerExplosion();
    void UpdateExplosion(float dt);

private:
    enum class State { Idle, Exploding, Done };
    State state = State::Idle;

    Texture2D explosionTexture;
    Animator explosiveAnim;

    float explosionSpeed = 500.0f;
    float explosionMaxRadius = 200.0f;
    float explosionForce = 100.0f;

    float explosionRadius = 0.0f;
    float explosionElapsed = 0.0f;
    Vector2 explosionCenter = { 0.0f, 0.0f };
    std::vector<Car*> affectedCars;
    std::unordered_set<Car*> affectedSet;

    bool pendingBodyDestroy = false;
};

// Puddle
class Puddle : public Obstacle
{
public:
    Puddle();

    void Start(const Vector2& spawnPoint) override;
    void OnCollision(PhysBody* physA, PhysBody* physB) override;
    void Draw() override;

private:
    float radius = 30.0f;           // px
};