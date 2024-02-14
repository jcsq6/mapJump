#ifndef COLLISION_H
#define COLLISION_H

#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

inline glm::vec2 rotate(glm::vec2 v, float angle)
{
	auto c = std::cos(angle);
	auto s = std::sin(angle);
	glm::vec2 res{
		c * v.x - s * v.y,
		s * v.x + c * v.y
	};

	return res;
}

struct polygon_view;

struct collision
{
	glm::vec2 mtv;
	glm::vec2 normal;
	bool collides;

	inline collision() : mtv{}, normal{}, collides{} {}

	inline operator bool() const
	{
		return collides;
	}
};

class polygon
{
public:
	polygon() : m_center{0, 0} {}

	template <typename T>
	inline polygon(std::initializer_list<glm::vec<2, T>> pts) : polygon()
	{
		assign(pts.begin(), pts.end());
	}

	template <typename InputIt>
	inline polygon(InputIt first, InputIt last) : polygon()
	{
		assign(first, last);
	}

	template <typename InputIt>
	inline void assign(InputIt first, InputIt last)
	{
		using vec_t = typename std::iterator_traits<InputIt>::value_type;
		static_assert(std::is_convertible_v<vec_t, glm::vec<2, float>>, "InputIt value_type must be convertible to a vec of size 2");

		m_pts.clear();
		m_center = {0, 0};

		if (first == last)
			return;
		
		std::size_t size = std::distance(first, last);
		m_pts.reserve(size);

		m_center += *first;
		m_pts.push_back(*(first++));
		for (; first != last; ++first)
		{
			m_center += *first;
			m_pts.push_back(*first);
		}

		m_center /= m_pts.size();
	}

	template <typename T>
	inline void assign(std::initializer_list<glm::vec<2, T>> pts) {	assign(pts.begin(), pts.end()); }

	glm::vec2 point(std::size_t i) const { return m_pts[i]; }

	glm::vec2 center() const { return m_center; }

	std::size_t size() const { return m_pts.size(); }

	auto begin() const { return m_pts.begin(); }
	auto end() const { return m_pts.end(); }

private:
	std::vector<glm::vec2> m_pts;
	glm::vec2 m_center;
};

struct polygon_view
{
	polygon_view() : offset{}, poly{nullptr}, scale{1.f, 1.f}, angle{0.f} {}
	polygon_view(const polygon &_poly) : offset{}, poly{&_poly}, scale{1.f, 1.f}, angle{0.f} {}
	polygon_view(const polygon &_poly, glm::vec2 _offset, glm::vec2 _scale, float _angle) : offset{_offset}, poly{&_poly}, scale{_scale}, angle{_angle} {}
	glm::vec2 offset;
	glm::vec2 scale;
	const polygon *poly;
	float angle;

	std::size_t size() const
	{
		return poly->size();
	}

	glm::vec2 point(std::size_t i) const
	{
		return transform(poly->point(i));
	}

	glm::vec2 normal(std::size_t i) const
	{
		glm::vec2 first = point(i);
		glm::vec2 second = point((i + 1) % poly->size());
		glm::vec2 perp{first.y - second.y, second.x - first.x};
		return glm::normalize(perp);
	}

	glm::vec2 center() const
	{
		return transform(poly->center());
	}

	glm::vec2 transform(glm::vec2 pt) const
	{
		// as if 
		// translate = {{1, 0, 0}, {0, 1, 0}, {offset.x, offset.y, 1}};
		// scale = {{scale.x, 0, 0}, {0, scale.y, 0}, {0, 0, 1}};
		// rotate = {{std::cos(angle), std::sin(angle), 0}, {-std::sin(angle), std::cos(angle), 0}, {0, 0, 1}};
		// transform = translate * rotate * scale_mat;
		// return transform * vec3((*poly)[i].pt, 1);
		auto c = std::cos(angle);
		auto s = std::sin(angle);
		glm::vec2 res{
			c * scale.x * pt.x - s * scale.y * pt.y + offset.x,
			s * scale.x * pt.x + c * scale.y * pt.y + offset.y
		};

		return res;
	}
};

inline glm::mat4 model(glm::vec2 offset, glm::vec2 scale, float angle)
{
	auto res = glm::scale(glm::rotate(glm::translate(glm::mat4(1.f), {offset, 0}), angle, {0, 0, 1}), {scale, 0});
	return res;
}

// returns collision with mtv to get a out of b or false if no collision
collision collides(const polygon_view &a, const polygon_view &b);

#ifdef MAPJUMP_DEBUG

#include <ostream>

template <glm::length_t size, typename T>
inline std::ostream &operator<<(std::ostream &stream, const glm::vec<size, T> &p)
{
	stream << '(';
	stream << p[0];
	for (int i = 1; i < size; ++i)
		stream << ", " << p[i];
	stream << ')';
	return stream;
}

// for use with geogebra
inline std::ostream &operator<<(std::ostream &stream, const polygon_view &p)
{
	stream << "Polygon(";
	if (p.size())
	{
		stream << p.point(0);
		for (std::size_t i = 1; i < p.size(); ++i)
			stream << ", " << p.point(i);
	}
	stream << ')';
	return stream;
}

#endif

#endif