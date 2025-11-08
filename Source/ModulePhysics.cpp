#include "Module.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModulePhysics.h"
#include "box2d/box2d.h"

#include "p2Point.h"

#include <math.h>

ModulePhysics::ModulePhysics(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	debug = true;
}

// Destructor
ModulePhysics::~ModulePhysics() {}

bool ModulePhysics::Start()
{
	printf("Creating Physics 2D environment\n");

	b2Vec2 gravity(0.0f, 0.0f);
	world = new b2World(gravity);
	world->SetContactListener(this);

	b2BodyDef bd;
	bd.type = b2_staticBody;
	ground = world->CreateBody(&bd);

	return true;
}

update_status ModulePhysics::PreUpdate()
{
	if (world != nullptr)
	{
		float dt = GetFrameTime();
		world->Step(dt, 6, 2);
	}

	for (b2Contact* c = world->GetContactList(); c; c = c->GetNext())
	{
		if (c->GetFixtureA()->IsSensor() && c->IsTouching())
		{
			b2BodyUserData data1 = c->GetFixtureA()->GetBody()->GetUserData();
			b2BodyUserData data2 = c->GetFixtureA()->GetBody()->GetUserData();

			PhysBody* pb1 = (PhysBody*)data1.pointer;
			PhysBody* pb2 = (PhysBody*)data2.pointer;
			if (pb1 && pb2 && pb1->listener)
				pb1->listener->OnCollision(pb1, pb2);
		}
	}


	return UPDATE_CONTINUE;
}

PhysBody* ModulePhysics::CreateRectangle(int x, int y, int width, int height, bool isSensor, Listener* listener, ColliderType ctype, bodyType type)
{
	b2BodyDef bodyDef;
	if (type == DYNAMIC) bodyDef.type = b2_dynamicBody;
	else if (type == KINEMATIC) bodyDef.type = b2_kinematicBody;
	else bodyDef.type = b2_staticBody;
	bodyDef.position.Set(PIXELS_TO_METERS(x), PIXELS_TO_METERS(y));

	b2Body* b = world->CreateBody(&bodyDef);

	b2PolygonShape box;
	box.SetAsBox(PIXELS_TO_METERS(width * 0.5f), PIXELS_TO_METERS(height * 0.5f));

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &box;
	fixtureDef.isSensor = isSensor;
	fixtureDef.friction = 0;
	fixtureDef.density = 1.0f;

	b->CreateFixture(&fixtureDef);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	pbody->ctype = ctype;
	pbody->listener = listener;

	b->GetUserData().pointer = (uintptr_t)pbody;

	return pbody;
}

PhysBody* ModulePhysics::CreateCircle(int x, int y, int radius, bool isSensor, Listener* listener, ColliderType ctype, bodyType type)
{
	b2BodyDef bodyDef;
	if (type == DYNAMIC) bodyDef.type = b2_dynamicBody;
	else if (type == KINEMATIC) bodyDef.type = b2_kinematicBody;
	else bodyDef.type = b2_staticBody;

	bodyDef.position.Set(PIXELS_TO_METERS(x), PIXELS_TO_METERS(y));
	bodyDef.angularDamping = 0.0f;

	b2Body* b = world->CreateBody(&bodyDef);

	b2CircleShape shape;
	shape.m_radius = PIXELS_TO_METERS(radius);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &shape;
	fixtureDef.isSensor = isSensor;
	fixtureDef.friction = 0.3f;
	fixtureDef.density = 1.0f;

	b->CreateFixture(&fixtureDef);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	pbody->ctype = ctype;
	pbody->listener = listener;

	b->GetUserData().pointer = (uintptr_t)pbody;

	return pbody;
}

PhysBody* ModulePhysics::CreateChain(int x, int y, int* points, int size, bool isSensor, Listener* listener, ColliderType ctype, bodyType type)
{
	b2BodyDef bodyDef;
	if (type == DYNAMIC) bodyDef.type = b2_dynamicBody;
	else if (type == KINEMATIC) bodyDef.type = b2_kinematicBody;
	else bodyDef.type = b2_staticBody;

	bodyDef.position.Set(PIXELS_TO_METERS(x), PIXELS_TO_METERS(y));
	b2Body* b = world->CreateBody(&bodyDef);

	const int count = size / 2;
	std::vector<b2Vec2> verts(count);
	for (int i = 0; i < count; ++i)
	{
		verts[i].x = PIXELS_TO_METERS(points[i * 2 + 0]);
		verts[i].y = PIXELS_TO_METERS(points[i * 2 + 1]);
	}

	b2ChainShape chain;
	chain.CreateLoop(verts.data(), count);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &chain;
	fixtureDef.isSensor = isSensor;
	fixtureDef.density = 1.0f;

	b->CreateFixture(&fixtureDef);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	pbody->ctype = ctype;
	pbody->listener = listener;

	b->GetUserData().pointer = (uintptr_t)pbody;

	return pbody;
}

PhysBody* ModulePhysics::CreateCar(float x, float y, float width, float height, float wheelRadius)
{
	// Create the chassis
	PhysBody* chassis = CreateRectangle(x, y, width, height, false, nullptr, ColliderType::UNKNOWN, DYNAMIC);
	chassis->body->SetAngularDamping(3.0f);
	chassis->body->SetLinearDamping(0.8f);

	// Calculate the positions of the wheels
	float offsetX = width * 0.4f;
	float offsetY = height * 0.5f;

	// Front wheels
	PhysBody* wheelFrontLeft = CreateCircle(x - offsetX, y - offsetY, wheelRadius, false, nullptr, ColliderType::WHEEL, DYNAMIC);
	PhysBody* wheelFrontRight = CreateCircle(x + offsetX, y - offsetY, wheelRadius, false, nullptr, ColliderType::WHEEL, DYNAMIC);

	// Rears wheels
	PhysBody* wheelBackLeft = CreateCircle(x - offsetX, y + offsetY, wheelRadius, false, nullptr, ColliderType::WHEEL, DYNAMIC);
	PhysBody* wheelBackRight = CreateCircle(x + offsetX, y + offsetY, wheelRadius, false, nullptr, ColliderType::WHEEL, DYNAMIC);

	// Joints
	b2RevoluteJointDef jointDef;
	jointDef.enableMotor = false;
	jointDef.enableLimit = true;
	jointDef.lowerAngle = 0;
	jointDef.upperAngle = 0;

	auto createJoint = [&](PhysBody* wheel)
		{
			jointDef.Initialize(chassis->body, wheel->body, wheel->body->GetWorldCenter());
			App->physics->world->CreateJoint(&jointDef);
		};

	// Creates the joints
	createJoint(wheelFrontLeft);
	createJoint(wheelFrontRight);
	createJoint(wheelBackLeft);
	createJoint(wheelBackRight);

	LOG("Car created successfully with 4 wheels and chassis");

	return chassis;
}

void ModulePhysics::SetBodyPosition(PhysBody* pbody, int x, int y, bool resetRotation)
{
	if (pbody == nullptr || pbody->body == nullptr)
		return;

	b2Vec2 newPos(PIXELS_TO_METERS(x), PIXELS_TO_METERS(y));
	float angle = resetRotation ? 0.0f : pbody->body->GetAngle();

	pbody->body->SetTransform(newPos, angle);
	pbody->body->SetLinearVelocity(b2Vec2(0, 0));
	pbody->body->SetAngularVelocity(0);
}

void ModulePhysics::BeginContact(b2Contact* contact)
{
	b2BodyUserData dataA = contact->GetFixtureA()->GetBody()->GetUserData();
	b2BodyUserData dataB = contact->GetFixtureB()->GetBody()->GetUserData();

	PhysBody* physA = (PhysBody*)dataA.pointer;
	PhysBody* physB = (PhysBody*)dataB.pointer;

	if (physA && physA->listener != NULL)
		physA->listener->OnCollision(physA, physB);

	if (physB && physB->listener != NULL)
		physB->listener->OnCollision(physB, physA);
}

void ModulePhysics::EndContact(b2Contact* contact)
{
	b2BodyUserData dataA = contact->GetFixtureA()->GetBody()->GetUserData();
	b2BodyUserData dataB = contact->GetFixtureB()->GetBody()->GetUserData();

	PhysBody* physA = (PhysBody*)dataA.pointer;
	PhysBody* physB = (PhysBody*)dataB.pointer;

	if (physA && physA->listener != NULL)
		physA->listener->EndCollision(physA, physB);

	if (physB && physB->listener != NULL)
		physB->listener->EndCollision(physB, physA);
}

update_status ModulePhysics::PostUpdate()
{
	if (IsKeyPressed(KEY_F1))
	{
		debug = !debug;
	}

	if (!debug)
		return UPDATE_CONTINUE;

	Vector2 mouse = GetMousePosition();
	b2Vec2 pMousePosition = b2Vec2(PIXELS_TO_METERS(mouse.x), PIXELS_TO_METERS(mouse.y));

	if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
	{
		for (b2Body* b = world->GetBodyList(); b; b = b->GetNext())
		{
			if (b->GetType() != b2_dynamicBody) continue;

			for (b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext())
			{
				if (f->TestPoint(pMousePosition))
				{
					mouseSelect = b;

					b2MouseJointDef def;
					def.bodyA = ground;
					def.bodyB = mouseSelect;
					def.target = pMousePosition;
					def.damping = 0.7f;
					def.stiffness = 20.f;
					def.maxForce = 100.0f * mouseSelect->GetMass();

					mouse_joint = (b2MouseJoint*)world->CreateJoint(&def);
					mouseSelect->SetAwake(true);
					break;
				}
			}
		}
	}

	if (mouse_joint && IsMouseButtonDown(MOUSE_BUTTON_LEFT))
	{
		mouse_joint->SetTarget(pMousePosition);

		// Dibujar la línea entre el mouse y el cuerpo
		b2Vec2 anchor = mouse_joint->GetBodyB()->GetPosition();
		DrawLine(
			METERS_TO_PIXELS(anchor.x),
			METERS_TO_PIXELS(anchor.y),
			mouse.x,
			mouse.y,
			RED
		);
	}

	if (mouse_joint && IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
	{
		world->DestroyJoint(mouse_joint);
		mouse_joint = nullptr;
		mouseSelect = nullptr;
	}

	// Bonus code: this will iterate all objects in the world and draw the circles
	// You need to provide your own macro to translate meters to pixels
	for (b2Body* b = world->GetBodyList(); b; b = b->GetNext())
	{
		for (b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext())
		{
			switch (f->GetType())
			{
				// Draw circles ------------------------------------------------
			case b2Shape::e_circle:
			{
				b2CircleShape* shape = (b2CircleShape*)f->GetShape();
				b2Vec2 pos = f->GetBody()->GetPosition();

				DrawCircle(METERS_TO_PIXELS(pos.x), METERS_TO_PIXELS(pos.y), (float)METERS_TO_PIXELS(shape->m_radius), Color{ 0, 0, 0, 128 });
			}
			break;

			// Draw polygons ------------------------------------------------
			case b2Shape::e_polygon:
			{
				b2PolygonShape* polygonShape = (b2PolygonShape*)f->GetShape();
				int32 count = polygonShape->m_count;
				b2Vec2 prev, v;

				for (int32 i = 0; i < count; ++i)
				{
					v = b->GetWorldPoint(polygonShape->m_vertices[i]);
					if (i > 0)
						DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), DARKBLUE);

					prev = v;
				}

				v = b->GetWorldPoint(polygonShape->m_vertices[0]);
				DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), DARKBLUE);
			}
			break;

			// Draw chains contour -------------------------------------------
			case b2Shape::e_chain:
			{
				b2ChainShape* shape = (b2ChainShape*)f->GetShape();
				b2Vec2 prev, v;

				for (int32 i = 0; i < shape->m_count; ++i)
				{
					v = b->GetWorldPoint(shape->m_vertices[i]);
					if (i > 0)
						DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), BLACK);
					prev = v;
				}

				v = b->GetWorldPoint(shape->m_vertices[0]);
				DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), BLACK);
			}
			break;

			// Draw a single segment(edge) ----------------------------------
			case b2Shape::e_edge:
			{
				b2EdgeShape* shape = (b2EdgeShape*)f->GetShape();
				b2Vec2 v1, v2;

				v1 = b->GetWorldPoint(shape->m_vertex0);
				v1 = b->GetWorldPoint(shape->m_vertex1);
				DrawLine(METERS_TO_PIXELS(v1.x), METERS_TO_PIXELS(v1.y), METERS_TO_PIXELS(v2.x), METERS_TO_PIXELS(v2.y), RED);
			}
			break;
			}
		}
	}

	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModulePhysics::CleanUp()
{
	printf("Destroying physics world\n");

	if (world != nullptr)
	{
		delete world;
		world = nullptr;
	}

	return true;
}