#pragma once
#include <vector>

#include "vec.h"

#include "shaders.h"
#include "application.h"

namespace graphics
{
    class object
    {
    public:
        object(application &app) {}
        
        virtual ~object() = default;

        virtual void draw() const = 0;
    };
}
