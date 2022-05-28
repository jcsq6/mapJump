#pragma once
#include "vec.h"

#include "shaders.h"

GRAPHICS_BEG
class object
{
public:
    virtual ~object() = default;

    virtual void draw() const = 0;
};
GRAPHICS_END
