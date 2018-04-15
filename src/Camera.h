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

	Camera(float fov_h, vec3 location, vec3 vup, vec3 lookat, float focus_dist, float aperture): location(location), focus_dist(focus_dist), aperture(aperture)
	{
		set_fov_h(fov_h);
		//focus_dist = length(location - lookat);
		//backness
		w = normalize(location - lookat);
		//rightness
		u = normalize(cross(w, vup));
		//downness
		v = cross(w, u);
	}

	Ray make_ray(ivec2 const & image_pos, Randomization rand) const
	{
		//offset from pixel top-left
		vec2 offset_imgplane(0.5, 0.5);
		switch (rand)
		{
		case Randomization::None: 
			break;
		case Randomization::MonteCarlo:
			offset_imgplane += linearRand(vec2(-0.5), vec2(0.5));
			break;
		default:
			break;
		}
		
		vec2 pos_imgplane = (vec2(image_pos) + offset_imgplane - half_img_size__);
		vec2 lensOffset = sample_in_disk(vec2(0.f), vec2(aperture * 0.5f));
		vec2 st = pos_imgplane * imageplane_dims__;
		vec3 pos_worldspace = location + focus_dist * (-w + u * st.x + v * st.y);
		return Ray(location + u * lensOffset.x + v * lensOffset.y, normalize(pos_worldspace - location - u * lensOffset.x - v * lensOffset.y));
	}

	void set_fov_h(float degrees)
	{
		if ((degrees < 0) || (degrees > 180)) return;
		fov_h = degrees;
		calc_image_plane();
	}

	void set_image_size(ivec2 const& size)
	{
		image_size = size;
		half_img_size__ = vec2(image_size) * 0.5f;
		pixel_scale__ = 1 / float(image_size.x);
		calc_image_plane();
	}
	
	float aperture{0};
	vec3 location;

private:

	void calc_image_plane()
	{
		float inv_aspect_ratio = image_size.y * 1.f / image_size.x;
		float half_width = tan(radians(fov_h * 0.5));
		imageplane_dims__ = vec2(half_width * 1.f/image_size.x, half_width * inv_aspect_ratio * 1.f/image_size.y) * 2.0f;
	}

	vec3 u, v, w;
	ivec2 image_size{ 512, 256 };
	vec2 half_img_size__;
	vec2 imageplane_dims__;
	float pixel_scale__;
	
	float focus_dist{ 1 };
	float fov_h{ 90 };
};