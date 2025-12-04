#include "Coroutine.h"
#include <algorithm>

int CoroutineManager::StartCoroutine(float duration, std::function<void()> callback)
{
    Coroutine c;
    c.id = nextId++;
    c.duration = duration;
    c.timer = 0.0f;
    c.finished = false;
    c.callback = callback;

    coroutines.push_back(c);
    return c.id;
}

void CoroutineManager::StopCoroutine(int id)
{
    for (auto& c : coroutines)
    {
        if (c.id == id && !c.finished)
        {
            c.finished = true;
            break;
        }
    }
}

void CoroutineManager::Update(float deltaTime)
{
    for (auto& c : coroutines)
    {
        if (!c.finished)
        {
            c.timer += deltaTime;
            if (c.timer >= c.duration)
            {
                if (c.callback)
                    c.callback();
                c.finished = true;
            }
        }
    }

    coroutines.erase(
        std::remove_if(coroutines.begin(), coroutines.end(),
            [](const Coroutine& c) { return c.finished; }),
        coroutines.end()
    );
}
