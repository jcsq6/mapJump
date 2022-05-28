#pragma once
#include <GLFW/glfw3.h>

class timer
{
    double beg;
    double tm;

public:
    inline timer() : beg{}, tm{} {}

    inline void start()
    {
        beg = glfwGetTime();
    }

    inline void stop()
    {
        double end = glfwGetTime();
        tm = end - beg;
    }

    inline double lap()
    {
        stop();
        return tm;
    }

    inline double seconds() const
    {
        return tm;
    }
};

class countdown
{
    timer t;
    double tm;
    // neccesary for it to say it's finished before it's started
    bool has_finished;
public:
    countdown() = default;
    countdown(double seconds) : tm{seconds}, has_finished{true} {}
    
    void set_time(double seconds)
    {
        tm = seconds;
    }

    void start()
    {
        has_finished = false;
        t.start();
    }

    void finish()
    {
        has_finished = true;
    }

    bool finished()
    {
        if (has_finished)
            return true;
        has_finished = t.lap() >= tm;
        return has_finished;
    }
};
