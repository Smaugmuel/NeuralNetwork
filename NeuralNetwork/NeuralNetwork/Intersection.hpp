#ifndef INTERSECTION_HPP
#define INTERSECTION_HPP
#include <SFML/System/Vector2.hpp>

struct IntersectionData
{
	bool isIntersecting = false;
	float distance = 0.0f;
};

/*
Perform a ray intersection test on a circle

param o : ray origin position
param d : ray normalized direction
param c : center of circle
param r : radius of circle

*/
IntersectionData RayVsCircle(sf::Vector2f o, sf::Vector2f d, sf::Vector2f c, float r)
{
	IntersectionData intersection;

	sf::Vector2f l = c - o;

	float s = l.x * d.x + l.y * d.y;
	float l2 = l.x * l.x + l.y * l.y;

	if (s < 0 && l2 > r * r)
	{
		// Missed the circle
		return intersection;
	}

	float m2 = l2 - s * s;

	if (m2 > r * r)
	{
		return intersection;
	}

	// Hit the circle
	float q = std::sqrtf(r * r - m2);
	float t = l2 > r * r ? s - q : s + q;

	intersection.isIntersecting = true;
	intersection.distance = t;

	return intersection;
}

#endif