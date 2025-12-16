#include "Animation.h"

Animator::Animator(Texture2D* texture, int frameWidth, int frameHeight)
    : texture(texture), frameWidth(frameWidth), frameHeight(frameHeight)
{
    if (texture)
        totalFrames = texture->width / frameWidth;
}

void Animator::AddAnim(const std::string& name, int startFrame, int frameCount, float fps, bool loop)
{
    clips[name] = { startFrame, frameCount, fps, loop };
}

void Animator::Play(const std::string& name, bool forceRestart)
{
    if (currentClipName != name || forceRestart) {
        currentClipName = name;
        playingReverse = false;
        timer = 0.0f;
        finished = false;
        currentFrame = clips[name].startFrame;
    }
}

void Animator::PlayReverse(const std::string& name, bool forceRestart)
{
    if (currentClipName != name || forceRestart) {
        currentClipName = name;
        playingReverse = true;
        timer = 0.0f;
        finished = false;
        currentFrame = clips[name].startFrame + clips[name].frameCount - 1;
    }
}

void Animator::StopAnim()
{
    finished = true;
    timer = 0.0f;
}

void Animator::Update(float deltaTime)
{
    if (currentClipName.empty() || !texture) return;

    AnimationClip& clip = clips[currentClipName];
    if (finished) return;

    timer += deltaTime;
    float frameProgress = timer * clip.fps;

    if (playingReverse)
    {
        int frameIndex = clip.frameCount - 1 - static_cast<int>(frameProgress);
        if (frameIndex < 0)
        {
            if (clip.loop)
                timer = 0.0f; // reiniciar
            else
            {
                frameIndex = 0;
                finished = true;
            }
        }
        currentFrame = clip.startFrame + frameIndex;
    }
    else
    {
        int frameIndex = static_cast<int>(frameProgress);
        if (clip.loop)
            frameIndex %= clip.frameCount;
        else if (frameIndex >= clip.frameCount)
        {
            frameIndex = clip.frameCount - 1;
            finished = true;
        }
        currentFrame = clip.startFrame + frameIndex;
    }
}

void Animator::Draw(Vector2 position, float scale, Color tint)
{
    if (!texture) return;

    Rectangle src = { (float)(currentFrame * frameWidth), 0, (float)frameWidth, (float)frameHeight };
    Rectangle dest = { position.x, position.y, frameWidth * scale, frameHeight * scale };
    Vector2 origin = { frameWidth * scale / 2, frameHeight * scale / 2 };

    DrawTexturePro(*texture, src, dest, origin, 0.0f, tint);
}

const std::string& Animator::GetCurrentAnimName() const
{
    return currentClipName;
}