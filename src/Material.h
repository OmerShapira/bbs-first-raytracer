#pragma once
#include <3rdparty/glm/glm.hpp>
#include <3rdparty/glm/gtc/random.hpp>
#include "ray.h"
#include "rt_math.h"

using namespace glm;

class Material
{
public:
	virtual bool Scatter(Ray const& ray_in, HitRecord const& rec, vec3& attenuation, Ray& ray_scattered) const = 0;
};


class Lambertian : public Material
{
public:
	Lambertian(vec3 Albedo) : Albedo(Albedo) {}
	bool Scatter(Ray const& ray_in, HitRecord const& rec, vec3& attenuation, Ray& ray_scattered) const override
	{
		vec3 direction = sample_in_sphere(vec3(0), vec3(1));
		//place it in a unit sphere

		direction += rec.normal;

		ray_scattered = Ray(rec.point, direction);
		attenuation = Albedo;
		return true;
	}
	
	vec3 Albedo;
};

class Metal : public Material
{
public:
	Metal(vec3 Albedo, float Roughness=0.f) : Albedo(Albedo), Roughness(Roughness) {}
	
	bool Scatter(Ray const& ray_in, HitRecord const& rec, vec3& attenuation, Ray& ray_scattered) const override
	{
		vec3 reflected = reflect(ray_in.direction, rec.normal);
		if (Roughness > 0)
		{
			//clamp
			float r = glm::min(1.f, Roughness);
			reflected += sample_in_sphere(rec.point + reflected, vec3(r, r, r));
			reflected = normalize(reflected);
		}
		ray_scattered = Ray(rec.point, reflected);
		attenuation = Albedo;
		return true;
	}
	
	float Roughness;
	vec3 Albedo;
};

class Dielectric : public Material
{
public:
	Dielectric(float Index):Index(Index) {}

	bool Scatter(Ray const& ray_in, HitRecord const& rec, vec3& attenuation, Ray& ray_scattered) const override
	{
		vec3 outward_normal;
		float ni_over_no;
		float reflected_prob, costheta;
		vec3 refracted; 
		attenuation = vec3(1.f);
		if (dot(ray_in.direction, rec.normal) > 0)
		{
			outward_normal = -rec.normal;
			ni_over_no = Index;
			costheta = Index * dot(ray_in.direction, rec.normal) / ray_in.direction.length();
		}
		else
		{
			outward_normal = rec.normal;
			ni_over_no = 1.f / Index;
			costheta = - Index * dot(ray_in.direction, rec.normal) / ray_in.direction.length();
		}
		if (refract(ray_in.direction, outward_normal, ni_over_no, refracted))
		{
			reflected_prob = schlick(costheta, Index);
		}
		else
		{
			reflected_prob = 1.0f;
		}

		vec3 out = (linearRand(0.f,1.f) < reflected_prob) ?
			reflect(ray_in.direction, rec.normal) : refracted;
		ray_scattered = Ray(rec.point, out);
		return true;
	}

	float Index;
};