#pragma once

#include <3rdparty/glm/glm.hpp>
#include <3rdparty/glm/gtc/random.hpp>
#include <3rdparty/glm/gtc/quaternion.hpp>
#include "ray.h"

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

		switch (rand)
		{
		case Randomization::None: 
			break;
		case Randomization::MonteCarlo:
			offset += linearRand(vec2(-0.5), vec2(0.5));
			break;
		default:
			break;
		}
		
		vec2 pos = ((vec2(image_pos) - half_img_size__) + offset) * pixel_scale__;
		//convert to screen space coords
		pos *= vec2(1, -1);
		//TODO (OS): Impl rotation
		vec3 origin = location - vec3(0, 0, dist_to_origin);
		vec3 direction = vec3(pos.x, pos.y, dist_to_origin);
		return Ray(origin, direction);
	}

	void set_fov_h(float degrees)
	{
		if ((degrees < 0) || (degrees > 180)) return;
		fov_h = degrees;
		float tg_half_fov = tan(radians(fov_h * 0.5));
		dist_to_origin = (scale * 0.5) / tg_half_fov;
	}

	void set_image_size(ivec2 const& size)
	{
		image_size = size;
		half_img_size__ = vec2(image_size) * 0.5f;
		pixel_scale__ = scale / float(image_size.x);
	}

	vec3 location, rotation;
	float clip_near, clip_far;

private:

	ivec2 image_size{ 512, 256 };
	vec2 half_img_size__;
	float pixel_scale__;
	
	float scale{ 1 };
	float fov_h{ 90 };
	float dist_to_origin;
};