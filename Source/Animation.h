#pragma once
#include "raylib.h"
#include <string>
#include <unordered_map>

struct AnimationClip {
    int startFrame;
    int frameCount;
    float fps;
    bool loop;
};

class Animator {
public:
    Animator() = default;
    Animator(Texture2D* texture, int frameWidth, int frameHeight);

    void AddAnim(const std::string& name, int startFrame, int frameCount, float fps, bool loop);
    void Play(const std::string& name, bool forceRestart = false);
    void PlayReverse(const std::string& name, bool forceRestart = false);
    void StopAnim();
    void Update(float deltaTime);
    void Draw(Vector2 position, float scale = 1.0f, Color tint = WHITE);

    bool IsFinished() const { return finished; }
    int GetCurrentFrame() const { return currentFrame; }
    const std::string& GetCurrentAnimName() const;

private:
    Texture2D* texture = nullptr;
    int frameWidth = 0;
    int frameHeight = 0;
    int totalFrames = 0;

    std::unordered_map<std::string, AnimationClip> clips;
    std::string currentClipName;

    float timer = 0.0f;
    int currentFrame = 0;
    bool playingReverse = false;
    bool finished = false;
};