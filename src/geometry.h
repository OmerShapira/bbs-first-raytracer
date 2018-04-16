#pragma once

#include <algorithm>
#include <memory>

#include <3rdparty\glm\glm.hpp>

#include "rt_math.h"
#include "Material.h"
#include "ray.h"

using namespace glm;
using std::shared_ptr;
using std::make_shared;
using std::static_pointer_cast;

namespace geometry
{

	class AABB 
	{
	public:
		AABB() : min__(vec3(NAN)), max__(vec3(NAN)) {}
		AABB (vec3 const& other_min, vec3 const& other_max)
		{
			min__ = other_min;
			max__ = other_max;
		}

		AABB Union(AABB const& other) const
		{
			return AABB(glm::min(other.min__, min__), glm::max(other.max__, max__));
		}

		bool Intersect(Ray const& ray, vec2 t_range) const
		{
			vec3 inv_d = 1.f / ray.direction;
			vec3 t_min = (min__ - ray.origin) * inv_d;
			vec3 t_max = (max__ - ray.origin) * inv_d;

			//TODO (OS): Optimize
			if (inv_d.x < 0)
			{
				std::swap(t_min.x, t_max.x);
			}
			if (inv_d.y < 0)
			{
				std::swap(t_min.y, t_max.y);
			}
			if (inv_d.z < 0)
			{
				std::swap(t_min.z, t_max.z);
			}
			
			t_min = glm::max(t_min, t_range.x);
			t_max = glm::min(t_max, t_range.y);

			//replaced this line because the discrete implementation is cheaper
			//if (glm::any(glm::lessThanEqual(t_max, t_min)))
			if ((t_max.x <= t_min.x) || (t_max.y <= t_min.y) || (t_max.z <= t_min.z))
			{
				return false;
			}
			return true;
		}

		vec3 max__, min__;
	};


	class Hitable
	{
	public:
		virtual bool Intersect(Ray const& ray, vec2 t_range, HitRecord& rec) const = 0;
		virtual AABB Bounds() const = 0;
		shared_ptr<Material> material;
	};

	class HitableList : public Hitable
	{
	public:

		bool Intersect(Ray const& Ray, vec2 t_range, HitRecord& rec) const override
		{
			bool hit_anything = false;

			for (shared_ptr<Hitable> const& hitable : list)
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

		AABB Bounds() const override
		{
			//TODO (OS): Implement this
			return AABB(vec3(FLT_MIN), vec3(FLT_MAX));
		}

		void Add(shared_ptr<Hitable> ptr)
		{
			list.push_back(ptr);
		}

		std::vector<shared_ptr<Hitable> > list;
	};

	class BVHNode : public Hitable
	{
	public:
		BVHNode(HitableList& list) : BVHNode(list.list.begin(), list.list.end(), 0) {}

		BVHNode(std::vector<shared_ptr<Hitable> >::iterator begin, std::vector<shared_ptr<Hitable> >::iterator end, uint depth)
		{
			if (end - begin == 0)
			{
				return;
			}
			if (end - begin == 1)
			{
				left = *begin;
				bounds = left->Bounds();
				return;
			}
			if (end - begin == 2)
			{
				left = *begin;
				right = * (begin + 1);
				bounds = left->Bounds().Union(right->Bounds());
				return;
			}

			auto cmp_x = [](shared_ptr<Hitable> & a, shared_ptr<Hitable> & b) -> bool 
			{ return a->Bounds().min__.x < b->Bounds().min__.x; };
			auto cmp_y = [](shared_ptr<Hitable> & a, shared_ptr<Hitable> & b) -> bool 
			{ return a->Bounds().min__.y < b->Bounds().min__.y; };
			auto cmp_z = [](shared_ptr<Hitable> & a, shared_ptr<Hitable> & b) -> bool 
			{ return a->Bounds().min__.z < b->Bounds().min__.z; };

			switch (depth % 3)
			{
			case 0: 
				std::nth_element(begin, begin + (end - begin) / 2, end, cmp_x);
				break;
			case 1:
				std::nth_element(begin, begin + (end - begin) / 2, end, cmp_y);
				break;
			case 3:
				std::nth_element(begin, begin + (end - begin) / 2, end, cmp_z);
				break;
			default:
				break;
			}

			left = std::make_shared<BVHNode>(begin, begin + ((end - begin) / 2), depth + 1);
			right = std::make_shared<BVHNode>(begin + ((end - begin) / 2) , end, depth + 1);
			bounds = left->Bounds().Union(right->Bounds());
		}

		bool Intersect(Ray const& ray, vec2 t_range, HitRecord& rec) const override
		{
			if (!bounds.Intersect(ray, t_range))
			{
				return false;
			}
			HitRecord hitrec_r, hitrec_l;
			bool hit_l = left && left->Intersect(ray, t_range, hitrec_l);
			bool hit_r = right && right->Intersect(ray, t_range, hitrec_r);
			if (hit_r && hit_l)
			{
				rec = (hitrec_l.t < hitrec_r.t) ? hitrec_l : hitrec_r;
				return true;
			}
			if (hit_l)
			{
				rec = hitrec_l;
				return true;
			}
			if (hit_r)
			{
				rec = hitrec_r;
				return true;
			}
			return false;
		}

		AABB Bounds() const override
		{
			return bounds;
		}
		
	private:
		AABB bounds;
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
}

