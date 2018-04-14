#pragma once
#include <iostream>
#include <memory>
#include <3rdparty/glm/vec3.hpp>
using glm::vec3;

class Material;

class Ray
{
public:
	Ray(vec3 const& origin, vec3 const& direction) : origin(origin), direction(direction) {}
	
	vec3 At(float t) const { return origin + t * direction; }

	vec3 origin, direction;

};

struct HitRecord
{
	float t;
	vec3 point;
	vec3 normal;
	std::shared_ptr<Material> mat;
};