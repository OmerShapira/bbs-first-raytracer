#pragma once
#include <3rdparty\glm\glm.hpp>
#include "rt_math.h"
#include "Material.h"
#include "ray.h"

using namespace glm;
using std::shared_ptr;

namespace geometry
{

	class AABB 
	{
	public:
		AABB() : min__(vec3(0.f)), max__(vec3(0.f)) {}
		AABB (vec3 const& other_min, vec3 const& other_max)
		{
			min__ = other_min;
			max__ = other_max;
		}

		AABB Union(AABB const& other) const
		{
			return AABB(glm::min(other.min__, min__), glm::max(other.max__, max__));
		}

		bool Intersect(Ray const& ray) const
		{
			vec3 inv_d = 1.f / ray.direction;
			vec3 t_min = (min__ - ray.origin) * inv_d;
			vec3 t_max = (max__ - ray.origin) * inv_d;

			//TODO (OS): Optimize
			if (inv_d.x < 0) std::swap(t_min.x, t_max.x);
			if (inv_d.y < 0) std::swap(t_min.y, t_max.y);
			if (inv_d.z < 0) std::swap(t_min.z, t_max.z);

			if (glm::any(glm::lessThanEqual(t_max, t_min)))
			{
				return false;
			}
			return true;
		}
	private:
		vec3 max__, min__;
	};


	class Hitable
	{
	public:
		virtual bool Intersect(Ray const& ray, vec2 t_range, HitRecord& rec) const = 0;
		virtual AABB Bounds() const = 0;
		shared_ptr<Material> material;
	};

	class BVHNode : Hitable
	{
	public:
		BVHNode() {}

		bool Intersect(Ray const& ray, vec2 t_range, HitRecord& rec) const override
		{

			return false;
		}
		AABB Bounds() const override
		{
			return bounds;
		}
		
	private:
		AABB bounds;
		//Should this be unique_ptr?
		shared_ptr<Hitable> left, right;
	};


	class Sphere : public Hitable
	{
	public:
		Sphere(vec3 const& center, float radius):
			center(center), radius(radius) {}

		~Sphere() {}

		AABB Bounds() const override
		{
			return AABB(vec3(center - radius), vec3(center + radius));
		}

		bool Intersect(Ray const& ray, vec2 t_range, HitRecord& rec) const override
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

		vec3 Normal(vec3 const& surface_point) const
		{
			return (surface_point - center) / radius;
		}

		vec3 center;
		float radius;
	};



	class HitableList : public Hitable
	{
	public:

		bool Intersect(Ray const& Ray, vec2 t_range, HitRecord& rec) const override
		{
			bool hit_anything = false;

			for (Sphere const& hitable : list)
			{
				HitRecord temp_hit_record;
				if (hitable.Intersect(Ray, t_range, temp_hit_record))
				{
					rec = temp_hit_record;
					t_range.y = min(t_range.y, temp_hit_record.t);
					hit_anything = true;
				}
			}

			return hit_anything;
		}

		AABB Bounds() const override
		{
			return AABB(vec3(FLT_MIN), vec3(FLT_MAX));
		}

		std::vector<Sphere> list;
	};
}

