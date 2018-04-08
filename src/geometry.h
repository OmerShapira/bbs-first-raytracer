#pragma once
#include <3rdparty\glm\glm.hpp>
#include "ray.h"

using namespace glm;
namespace geometry
{

float sum_parts(vec3 const& v)
{
	return v.x + v.y + v.z;
}

class Hitable 
{
public:
	virtual float intersect(Ray const& Ray) = 0;
	virtual vec3 normal(vec3 const& surface_point) = 0;
};

class HitableList : public Hitable
{
private:
	std::vector<std::shared_ptr<Hitable> > list;
};

class Sphere : public Hitable
{
	
public:

	Sphere(vec3 const& center, float radius) :
		center(center), radius(radius) {}
	~Sphere() {}

public:

	float intersect(Ray const& ray) override
	{
		float a = sum_parts(ray.direction * ray.direction);
		float b = sum_parts((ray.origin - center) * ray.direction) * 2.0;
		vec3 c_vecpart = (ray.origin - center);
		float c = sum_parts(c_vecpart * c_vecpart) - radius * radius;

		float discriminant = b * b - 4.0 * a * c;
		if (discriminant < 0)
		{
			return -1.0;
		}
		else
		{
			return (-b - sqrt(discriminant)) / (2.0 * a);
		}
	}

	vec3 normal(vec3 const& surface_point) override
	{
		return surface_point - center;
	}

	vec3 center;
	float radius;
};

}

