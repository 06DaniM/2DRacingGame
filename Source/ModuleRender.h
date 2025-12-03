#pragma once
#include "Module.h"
#include "Globals.h"
#include <functional>

class Player;

class ModuleRender : public Module
{
public:
    ModuleRender(Application* app, bool start_enabled = true);
    ~ModuleRender();

    bool Init() override;
    update_status PreUpdate() override;
    update_status Update() override;
    update_status PostUpdate() override;
    bool CleanUp() override;

    void SetBackgroundColor(Color color);
    void SetPlayer(Player* p) { player = p; }

    bool Draw(Texture2D texture, int x, int y, const Rectangle* section = nullptr, double angle = 0, int pivot_x = 0, int pivot_y = 0) const;
    bool DrawText(const char* text, int x, int y, Font font, int spacing, Color tint) const;

    Camera2D& GetCamera() { return camera; }
    std::function<void()> DrawInsideCamera;
    std::function<void()> DrawAfterBegin;

private:
    Color background;
    Camera2D camera;
    Player* player = nullptr;
};
