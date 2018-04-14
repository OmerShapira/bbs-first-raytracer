#pragma once
#include "3rdparty\glm\glm.hpp"
#include "3rdparty\glm\gtc\random.hpp"

using glm::vec3;

float sum_parts(vec3 const& v)
{
	return v.x + v.y + v.z;
}

vec3 reflect(vec3 in, vec3 normal)
{
	return in - 2.f * dot(in, normal) * normal;
}

bool refract(vec3 in, vec3 normal, float ni_over_no, vec3& refracted)
{
	float h = dot(normalize(in), normal);
	float discriminant = 1 - ni_over_no * ni_over_no * (1 - h * h);
	if (discriminant > 0)
	{
		refracted = ni_over_no * (in - normal * h) - normal * sqrt(discriminant);
		return true;
	}
	else
	{
		return false;
	}
}


vec3 sample_in_sphere(vec3 center, vec3 radius)
{
	vec3 out;
	do 
	{ 
		out = linearRand(vec3(-1), vec3(1)); 
	} while (dot(out, out) >= 1);
	return out * radius + center;
}