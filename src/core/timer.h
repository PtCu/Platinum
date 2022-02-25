#include <iostream>
#include <thread>
#include <chrono>
#include <string>
class Timer
{
public:
    std::chrono::time_point<std::chrono::steady_clock> start, end;
    std::chrono::duration<float> duration;
    Timer(const std::string& name):_name(name)
    {
        start = std::chrono::high_resolution_clock::now();
    }
    ~Timer()
    {
        end = std::chrono::high_resolution_clock::now();
        duration = end - start;
        float ms = duration.count() * 1000.0f;
        std::cout <<"Function " <<_name<<" took " << ms << "ms" << std::endl;
    }
    const std::string _name;
};

