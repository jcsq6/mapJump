#include "collision.h"
		

bool project_onto(const polygon_view &a, const polygon_view &b, float &min_intersection, collision &res)
{
	for (std::size_t a_edge = 0; a_edge < a.poly->size(); ++a_edge)
	{
		glm::vec2 normal = a.normal(a_edge);

		float amin = std::numeric_limits<float>::infinity();
		float amax = -std::numeric_limits<float>::infinity();
		for (std::size_t i = 0; i < a.size(); ++i)
		{
			float cur = glm::dot(normal, a.point(i));
			if (cur < amin)
				amin = cur;
			if (cur > amax)
				amax = cur;
		}

		float bmin = std::numeric_limits<float>::infinity();
		float bmax = -std::numeric_limits<float>::infinity();
		for (std::size_t i = 0; i < b.size(); ++i)
		{
			float cur = glm::dot(normal, b.point(i));
			if (cur < bmin)
				bmin = cur;
			if (cur > bmax)
				bmax = cur;
		}

		if (amax < bmin || bmax < amin)
			return false;

		float cur_intersection = std::min(amax - bmin, bmax - amin);
		
		// if this intersection is less than the minimum
		if (cur_intersection < min_intersection)
		{
			min_intersection = cur_intersection;
			res.mtv = normal * min_intersection;
			res.normal = normal;
		}
	}

	return true;
}

// returns collision with mtv to get a out of b or false if no collision
collision collides(const polygon_view &a, const polygon_view &b)
{
	collision res;
	res.collides = true;
	float intersection = std::numeric_limits<float>::infinity();
	if (project_onto(a, b, intersection, res))
	{
		if (project_onto(b, a, intersection, res))
		{
			glm::vec2 center_diff = b.center() - a.center();
			if (glm::dot(res.normal, center_diff) > 0)
			{
				res.mtv *= -1;
				res.normal *= -1;
			}
			return res;
		}
	}
	return {};
}