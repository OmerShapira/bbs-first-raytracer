#pragma once
#include <3rdparty/glm/vec3.hpp>
using glm::vec3;

class Ray
{
public:
	Ray(vec3 const& origin, vec3 const& direction) : origin(origin), direction(direction) {}
	
	vec3 at(float t) const { return origin + t * direction; }

	vec3 origin, direction;

};

