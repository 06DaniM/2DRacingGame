#include "ModuleRender.h"
#include "Application.h"
#include "Player.h"
#include <raylib.h>

#define WORLD_WIDTH  7000
#define WORLD_HEIGHT 4500

ModuleRender::ModuleRender(Application* app, bool start_enabled) : Module(app, start_enabled)
{
    background = RAYWHITE;
}

ModuleRender::~ModuleRender() {}

bool ModuleRender::Init()
{
    LOG("Creating Renderer context");

    camera.offset = { SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = 1.0f;
    camera.target = { 0,0 };

    return true;
}

update_status ModuleRender::PreUpdate()
{
    return UPDATE_CONTINUE;
}

update_status ModuleRender::Update()
{
    BeginDrawing();
    ClearBackground(background);

    // Set the camera inside the map
    if (player)
    {
        int px, py;
        player->pbody->GetPosition(px, py);

        float targetX = (float)px;
        float targetY = (float)py;

        float halfScreenWidth = SCREEN_WIDTH * 0.5f / camera.zoom;
        float halfScreenHeight = SCREEN_HEIGHT * 0.5f / camera.zoom;

        // Clamp X
        if (targetX < halfScreenWidth)
            targetX = halfScreenWidth;
        else if (targetX > WORLD_WIDTH - halfScreenWidth)
            targetX = WORLD_WIDTH - halfScreenWidth;

        // Clamp Y
        if (targetY < halfScreenHeight)
            targetY = halfScreenHeight;
        else if (targetY > WORLD_HEIGHT - halfScreenHeight)
            targetY = WORLD_HEIGHT - halfScreenHeight;

        camera.target = { targetX, targetY };
    }

    if (DrawInsideCamera)
    {
        BeginMode2D(camera);
        DrawInsideCamera();
        EndMode2D();
    }

    return UPDATE_CONTINUE;
}

update_status ModuleRender::PostUpdate()
{
    if (DrawAfterBegin)
        DrawAfterBegin();

    DrawFPS(10, 10);
    EndDrawing();
    return UPDATE_CONTINUE;
}

bool ModuleRender::CleanUp()
{
    return true;
}

void ModuleRender::SetBackgroundColor(Color color)
{
    background = color;
}

// Draw to screen
bool ModuleRender::Draw(Texture2D texture, int x, int y, const Rectangle* section, double angle, int pivot_x, int pivot_y) const
{
    Vector2 position = { (float)x - pivot_x, (float)y - pivot_y };
    Rectangle rect = { 0.f, 0.f, (float)texture.width, (float)texture.height };

    if (section != nullptr) rect = *section;

    DrawTextureRec(texture, rect, position, WHITE);
    return true;
}

bool ModuleRender::DrawText(const char* text, int x, int y, Font font, int spacing, Color tint) const
{
    Vector2 position = { (float)x, (float)y };
    DrawTextEx(font, text, position, (float)font.baseSize, (float)spacing, tint);
    return true;
}