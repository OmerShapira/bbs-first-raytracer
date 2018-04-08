#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <time.h>

#define STB_IMAGE_IMPLEMENTATION
#include <3rdparty/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <3rdparty/stb_image_write.h>
#include <3rdparty/glm/vec3.hpp>

#include <ray.h>
#include <geometry.h>
#include <Camera.h>

using std::shared_ptr;
using std::make_shared;
using namespace geometry;

std::vector< shared_ptr<Hitable> > primitives;

inline vec3 lerp(vec3 const v1, vec3 const v2, float t)
{
	return v1 * (1 - t) + v2 * t;
}

vec3 color(Ray const& r)
{
	//try to intersect some objects for visibility
	for (shared_ptr<Hitable> const p : primitives)
	{
		float intersection = p->intersect(r);
		if (intersection > 0)
		{
			vec3 normal = p->normal(r.at(intersection));
			normal /= normal.length();
			normal = (normal * vec3(0.5) + vec3(0.5));
			return normal;
		}
	}

	//let's apply some minimal shading
	vec3 dir = r.direction;
	dir /= r.direction.length();
	float t = 0.5 * (dir.y + 1.0);
	vec3 sky = lerp(vec3(1, 1, 1), vec3(.5, .7, 1), t);
	return sky;
}

vec3 sample(Camera const& camera, ivec2 const& pos, int const num_samples, Randomization const randomization)
{
	vec3 accum;
	for (int i = 0; i < num_samples; ++i)
	{
		Ray r = camera.make_ray(pos, randomization);
		accum += color(r);
	}
	accum *= 1.0f / num_samples;
	return accum;
}

int trace(int w, int h, unsigned char * img)
{
	Camera camera;
	//camera.location.z = ;
	camera.set_image_size(ivec2(w, h));
	for (int j = 0; j < h; ++j)
	{
		for (int i = 0; i < w; ++i)
		{
			vec3 c = sample(camera, ivec2(i, j), 8, Randomization::MonteCarlo);
			img[(j*w + i) * 3 + 0] = c.r * 255;
			img[(j*w + i) * 3 + 1] = c.g * 255;
			img[(j*w + i) * 3 + 2] = c.b * 255;
		}
	}
	return 0;
}

int main()
{
	float cur = clock();
	float prev = cur;

	int const w = 512, h = 256;
	unsigned char *img = new unsigned char[w * h * 3];

	primitives.push_back(make_shared<Sphere>(vec3(0, 3, 10), 3));
	primitives.push_back(make_shared<Sphere>(vec3(0, -100, 0), 100));
	
	int result;
	prev = clock();

	result = trace(w, h, img);

	cur = clock();
	std::cout << std::to_string(prev - cur) << std::endl;
	prev = cur;

	stbi_write_png("image.png", w, h, 3, img, w*3);

	cur = clock();
	std::cout << std::to_string(prev - cur) << std::endl;

	delete[] img;
	return 0;
}