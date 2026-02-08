#pragma once

#include <glm/glm.hpp>

class Ball; // fwd

struct BallFliedOff {
	Ball &ball;
};

struct PlayerMoved { 
	glm::vec2 position;
};