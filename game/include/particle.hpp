#pragma once

#include <glm/glm.hpp>

struct Particle {
	glm::vec2 position;
	glm::vec2 velocity;
	glm::vec4 color;
	float lifeTime;

	Particle() : position(0.0f), velocity(0.0f), color(1.0f), lifeTime(0.0f) { }
};