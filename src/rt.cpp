#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <time.h>
#include <omp.h>

#define STB_IMAGE_IMPLEMENTATION
#include <3rdparty/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <3rdparty/stb_image_write.h>
#include <3rdparty/glm/vec3.hpp>
#include <3rdparty/glm/gtc/random.hpp>

#include "rt_math.h"
#include <ray.h>
#include <geometry.h>
#include <Camera.h>
#include <Material.h>

using std::shared_ptr;
using std::make_shared;
using namespace geometry;

HitableList world;

inline vec3 lerp(vec3 const v1, vec3 const v2, float t)
{
	return v1 * (1 - t) + v2 * t;
}

vec3 color(Ray const& r, Hitable *world, int recursion_num)
{
	//try to intersect some objects for visibility
	HitRecord rec;
	bool intersection = world->Intersect(r, vec2(0.001, FLT_MAX), rec);
	//FIXME (OS): Magic number
	if (intersection)
	{
		//vec3 normal = rec.normal * vec3(0.5) + vec3(0.5);
		//return normal;
		
		Ray scattered(vec3(0), vec3(0));
		vec3 attenuation;
		bool does_scatter = rec.mat->Scatter(r, rec, attenuation, scattered);
		if (does_scatter && (recursion_num < 16))
		{
			return attenuation * color(scattered, world, recursion_num + 1);
		}
		else
		{
			return vec3(0);
		}
	}

	//let's apply some minimal shading
	vec3 dir = r.direction;

	dir /= sqrt(dot(r.direction, r.direction));
	float t = 0.5 * (dir.y + 1.0);
	vec3 sky = r.direction * vec3(.5f) + vec3(.5f);
	//vec3 sky = lerp(vec3(1, 1, 1), vec3(.5, .7, 1), t);
	return sky;
}


vec3 sample(Camera const& camera, ivec2 const& pos, int const num_samples, Randomization const randomization)
{
	vec3 accum;
	for (int i = 0; i < num_samples; ++i)
	{
		Ray r = camera.make_ray(pos, randomization);
		accum += color(r, &world, 0);
	}
	accum *= 1.0f / num_samples;
	return accum;
}

int trace(int w, int h, unsigned char * img)
{
	Camera camera(90.f, vec3(0., 0., -2.f), vec3(0.,1.,0.), vec3(0., 0., 0), 0.05);
	camera.set_image_size(ivec2(w, h));
	for (int j = 0; j < h; ++j)
	{
		#pragma omp parallel for
		for (int i = 0; i < w; ++i)
		{
			vec3 c = sample(camera, ivec2(i, j), 32, Randomization::MonteCarlo);
			c = sqrt(c);
			//magic number for float truncation
			img[(j*w + i) * 3 + 0] = int(c.r * 255.99);
			img[(j*w + i) * 3 + 1] = int(c.g * 255.99);
			img[(j*w + i) * 3 + 2] = int(c.b * 255.99);
		}
	}
	return 0;
}

int main()
{
	int const w = 512, h = 256;
	unsigned char *img = new unsigned char[w * h * 3];

	//Hero
	auto s1 = make_shared<Sphere>(vec3(0, 0, 0), 0.5f);
	s1->material = make_shared<Lambertian>(vec3(0.7, 0.2, 0.2));
	//Floor
	auto s2 = make_shared<Sphere>(vec3(0, -100.5f, 0), 100);
	s2->material = make_shared<Lambertian>(vec3(0.1, 0.1, 0.8));
	
	auto s3 = make_shared<Sphere>(vec3(1, 0, 0), 0.5f);
	s3->material = make_shared<Metal>(vec3(0.8, 0.8, 0.8), 0.05);
	auto s4 = make_shared<Sphere>(vec3(-1.5, 0, 2), 1.f);
	s4->material = make_shared<Metal>(vec3(0.8, 0.6, 0.1));
	
	auto s5 = make_shared<Sphere>(vec3(-0.5, -0.25, -0.5), 0.25f);
	s5->material = make_shared<Dielectric>(1.33);

	world.Add(s1);
	world.Add(s2);
	world.Add(s3);
	world.Add(s4);
	world.Add(s5);

	int result;
	result = trace(w, h, img);

	stbi_write_png("image.png", w, h, 3, img, w*3);

	delete[] img;
	return 0;
}