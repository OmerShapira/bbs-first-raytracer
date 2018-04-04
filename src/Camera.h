#pragma once

#include <3rdparty/glm/glm.hpp>
#include <3rdparty/glm/gtc/random.hpp>
#include "ray.h"
//#include <3rdparty/glm/>

using namespace glm;

enum class Randomization
{
	None, 
	MonteCarlo
};

class Camera
{
public:

	Camera()
	{
		set_fov_h(90);
	}

	Ray make_ray(ivec2 const & image_pos, Randomization rand) const
	{
		//offset from pixel top-left
		vec2 offset(0.5, 0.5);
		if (rand == Randomization::MonteCarlo)
		{
			offset += linearRand(vec2(-0.5), vec2(0.5));
		}
		vec2 pos = ((vec2(image_pos) - vec2(image_size) * 0.5f) + offset) * scale / float(image_size.x);
		//TODO (OS): Impl rotation
		vec3 origin = location - vec3(0, 0, dist_to_origin);
		vec3 direction = vec3(pos.x, pos.y, 0) - vec3(0, 0, dist_to_origin);
		return Ray(origin, direction);
	}

	void set_fov_h(float degrees)
	{
		if ((degrees < 0) || (degrees > 180)) return;
		fov_h = degrees;
		float tg_half_fov = tan(radians(fov_h * 0.5));
		dist_to_origin = (scale * 0.5) / tg_half_fov;
	}

	vec3 location, rotation;
	ivec2 image_size{ 512, 256 };
	float clip_near, clip_far;

private:
	float scale{ 1 };
	float fov_h{ 90 };
	float dist_to_origin;
};