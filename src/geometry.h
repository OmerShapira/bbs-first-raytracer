#pragma once
#include <3rdparty\glm\glm.hpp>
#include "rt_math.h"
#include "Material.h"
#include "ray.h"

using namespace glm;
using std::shared_ptr;

namespace geometry
{


class Hitable 
{
public:
	virtual bool Intersect(Ray const& ray, vec2 t_range, HitRecord& rec) = 0;
	shared_ptr<Material> material;
};

class HitableList : public Hitable
{
public:

	bool Intersect(Ray const& Ray, vec2 t_range, HitRecord& rec)
	{
		bool hit_anything = false;

		for (shared_ptr<Hitable> hitable : list)
		{
			HitRecord temp_hit_record;
			if (hitable->Intersect(Ray, t_range, temp_hit_record))
			{
				rec = temp_hit_record;
				t_range.y = min(t_range.y, temp_hit_record.t);
				hit_anything = true;
			}
		}

		return hit_anything;
	}
	void Add(std::shared_ptr<Hitable> const& hitable)
	{
		list.push_back(hitable);
	}
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

	bool Intersect(Ray const& ray, vec2 t_range,  HitRecord& rec) override
	{
		float a = sum_parts(ray.direction * ray.direction);
		float b = sum_parts((ray.origin - center) * ray.direction) * 2.0;
		vec3 c_vecpart = (ray.origin - center);
		float c = sum_parts(c_vecpart * c_vecpart) - radius * radius;

		float discriminant = b * b - 4.0 * a * c;
		if (discriminant < 0)
		{
			return false;
		}
		else
		{
			float t = (-b - sqrt(discriminant)) / (2.0 * a);
			if (t > t_range.x && t < t_range.y)
			{
				rec.t = t;
				rec.point = ray.At(rec.t);
				rec.normal = Normal(rec.point);
				rec.mat = material;
				return true;

			}
			t = (-b + sqrt(discriminant)) / (2.0 * a);
			if (t > t_range.x && t < t_range.y)
			{
				rec.t = t;
				rec.point = ray.At(rec.t);
				rec.normal = Normal(rec.point);
				rec.mat = material;
				return true;
			}
		}
	}

	vec3 Normal(vec3 const& surface_point)
	{
		return (surface_point - center)/radius;
	}

	vec3 center;
	float radius;
};

}

