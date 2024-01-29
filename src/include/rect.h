#ifndef RECT_H
#define RECT_H

#include <glm/vec2.hpp>

struct rect
{
	glm::vec2 min;
	glm::vec2 dims;

	glm::vec2 max() const { return min + dims; }
};

#endif