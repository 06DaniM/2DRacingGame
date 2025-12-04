#pragma once
#include <functional>
#include <vector>

struct Coroutine
{
    int id;
    float duration;
    float timer;
    bool finished;
    std::function<void()> callback;
};

class CoroutineManager
{
public:
    int StartCoroutine(float duration, std::function<void()> callback);
    void StopCoroutine(int id);
    void Update(float deltaTime);

private:
    std::vector<Coroutine> coroutines;
    int nextId = 0;
};
