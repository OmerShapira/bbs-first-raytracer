#pragma once
#include "ray.h"
#include <3rdparty/glm/gtc/random.hpp>

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
		vec3 direction;
		//place it in a unit sphere
		do {
			direction = linearRand(vec3(-1), vec3(1));
		} while (dot(direction, direction) >= 1);

		direction += rec.normal;

		ray_scattered = Ray(rec.point, direction);
		attenuation = Albedo;
		return true;
	}
	
	vec3 Albedo;
};