#include <iostream>
#include <vector>
#include <memory>

#define STB_IMAGE_IMPLEMENTATION
#include <3rdparty/stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <3rdparty/stb_image_write.h>
#include <3rdparty/glm/vec3.hpp>

#include <ray.h>
#include <geometry.h>

using glm::vec3;
using std::shared_ptr;
using std::make_shared;
using namespace geometry;

std::vector< shared_ptr<Primitive> > primitives;

inline vec3 lerp(vec3 const v1, vec3 const v2, float t)
{
	return v1 * (1 - t) + v2 * t;
}

vec3 color(Ray const& r)
{
	//try to intersect some objects for visibility
	for (shared_ptr<Primitive> const p : primitives)
	{
		if (p->intersect(r) > 0)
		{
			
			return (vec3(1, 0, 0));
		}
	}

	//let's apply some minimal shading
	vec3 dir = r.direction;
	dir /= r.direction.length();
	float t = 0.5 * (dir.y + 1.0);
	return lerp(vec3(1, 1, 1), vec3(.5, .7, 1), t);
}

int trace(int w, int h, unsigned char * img)
{
	for (int j = 0; j < h; ++j)
	{
		for (int i = 0; i < w; ++i)
		{
			//shoot ray from origin to screen
			float tx = i * 1.0 / w;
			float ty = j * 1.0 / h;
			float x = (tx - 0.5) * 2.0 * 2.0;
			float y = (ty - 0.5) * 1.0 * 2.0;
			Ray r = Ray(vec3(0, 0, 0), vec3(x, y, -1.0));
			//evaluate its color
			vec3 c = color(r);
			img[(j*w + i) * 3 + 0] = c.r * 255;
			img[(j*w + i) * 3 + 1] = c.g * 255;
			img[(j*w + i) * 3 + 2] = c.b * 255;
		}
	}
	return 0;
}

int main()
{
	int const w = 512, h = 256;
	unsigned char *img = new unsigned char[w * h * 3];

	primitives.push_back(make_shared<Sphere>(vec3(0, 0, -10), 3));
	primitives.push_back(make_shared<Sphere>(vec3(5, 0, -10), 1));

	int result;
	result = trace(w, h, img);

	stbi_write_png("image.png", w, h, 3, img, w*3);
	delete[] img;
	return 0;
}