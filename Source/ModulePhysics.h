#pragma once

#include "Module.h"
#include "Globals.h"
#include "Listener.h"
#include "box2d/box2d.h"
#include <vector>
#include <string>

enum bodyType {
    DYNAMIC,
    STATIC,
    KINEMATIC
};

enum class ColliderType {
    PLAYER,
    AICAR,
    WHEEL,
    UI,
    CHECKEREDFLAG,
    CHECKPOINT,
    UNKNOWN
};

class PhysBody
{
public:
    PhysBody() : body(nullptr), listener(nullptr), ctype(ColliderType::UNKNOWN) {}
    ~PhysBody() {}

    void GetPosition(int& x, int& y) const
    {
        if (body)
        {
            b2Vec2 pos = body->GetPosition();
            x = METERS_TO_PIXELS(pos.x);
            y = METERS_TO_PIXELS(pos.y);
        }
    }

    float GetRotation() const
    {
        return body ? RADTODEG * body->GetAngle() : 0.0f;
    }

    bool Contains(int x, int y) const
    {
        if (!body) return false;

        for (b2Fixture* f = body->GetFixtureList(); f; f = f->GetNext())
        {
            if (f->GetShape()->TestPoint(body->GetTransform(), b2Vec2(PIXELS_TO_METERS(x), PIXELS_TO_METERS(y))))
                return true;
        }
        return false;
    }

    void SetSensor(bool isSensor)
    {
        if (!body) return;

        for (b2Fixture* f = body->GetFixtureList(); f; f = f->GetNext())
        {
            f->SetSensor(isSensor);
        }
    }

    void Draw(int width, int height, Color color) const
    {
        if (!body) return;

        int x, y;
        GetPosition(x, y);

        float rotation = GetRotation();

        DrawRectanglePro(
            { (float)x, (float)y, (float)width, (float)height },
            { (float)width / 2, (float)height / 2 },
            rotation,
            color
        );
    }

public:
    b2Body* body = nullptr;
    ColliderType ctype;
    bodyType btype;
    Listener* listener;

    int isActive = true;

    Texture2D carTexture;
    std::string id;
    int n;

    bool selectable = true;
};

// Module --------------------------------------
class ModulePhysics : public Module, public b2ContactListener
{
public:
    ModulePhysics(Application* app, bool start_enabled = true);
    ~ModulePhysics();

    bool Start() override;
    update_status PreUpdate() override;
    update_status PostUpdate() override;
    bool CleanUp() override;

    PhysBody* CreateRectangle(int x, int y, int width, int height, float angle, bool isSensor, Listener* listener, ColliderType ctype, bodyType type);
    PhysBody* CreateCircle(int x, int y, int radius, bool isSensor, Listener* listener, ColliderType ctype = ColliderType::UNKNOWN, bodyType type = bodyType::DYNAMIC);
    PhysBody* CreateChain(int x, int y, int* points, int size, bool isSensor, Listener* listener, ColliderType ctype = ColliderType::UNKNOWN, bodyType type = bodyType::STATIC);
    PhysBody* CreateCar(float x, float y, float width, float height, float wheelRadius, std::vector<PhysBody*>& carParts, std::vector<b2Joint*>& carJoints);

    void SetBodyPosition(PhysBody* pbody, int x, int y, bool resetRotation);
    std::vector<b2Fixture*> GetFixtures();

    void DestroyBody(PhysBody* pbody);
    void DestroyJoint(b2Joint* joint);
    
    void BeginContact(b2Contact* contact);
    void EndContact(b2Contact* contact);

private:
    b2World* world = nullptr;

    bool debug = false;
    b2Body* ground = nullptr;
    b2Body* mouseSelect = nullptr;
    b2MouseJoint* mouse_joint = nullptr;
};
