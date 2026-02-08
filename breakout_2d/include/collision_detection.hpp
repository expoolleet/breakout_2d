#pragma once

#include "ball.hpp"
#include "game_object.hpp"
#include "collision_type.hpp"

#include <glm/glm.hpp>
#include <tuple>

namespace CollisionDetection {
	inline Collision NoneCollision = std::tuple(false, COLLISION_DIRECTION_NONE, glm::vec2(0.0f));

	inline CollisionDirection _getCollisionDirection(glm::vec2 target) {
		static glm::vec2 directions[4] = {
			glm::vec2(0.0f, 1.0f),
			glm::vec2(1.0f, 0.0f),
			glm::vec2(0.0f, -1.0f),
			glm::vec2(-1.0f, 0.0f),
		};

		float maxContribution = 0.0f;
		unsigned int bestMatch = -1;
		for (unsigned int i = 0; i < 4; ++i) {
			float cosAngle = glm::dot(directions[i], glm::normalize(target));
			if (cosAngle > maxContribution) {
				maxContribution = cosAngle;
				bestMatch = i;
			}
		}
		return (CollisionDirection)bestMatch;
	}

	// AABB
	inline Collision checkCollision(GameObject &a, GameObject &b) {
		bool xCollision = a.getPosition().x + a.getSize().x >= b.getPosition().x &&
			b.getPosition().x + b.getSize().x >= a.getPosition().x;
		bool yCollision = a.getPosition().y + a.getSize().y >= b.getPosition().y &&
			b.getPosition().y + b.getSize().y >= a.getPosition().y;
		return std::tuple(xCollision && yCollision, COLLISION_DIRECTION_UP, glm::vec2(0.0f));
	}

	// AABB with circle object
	inline Collision checkCollision(Ball &ball, GameObject &gameObject) {
		glm::vec2 aabbHalf = glm::vec2(gameObject.getSize().x / 2, gameObject.getSize().y / 2);
		glm::vec2 aabbCenter = glm::vec2(gameObject.getPosition().x + aabbHalf.x, gameObject.getPosition().y + aabbHalf.y);

		glm::vec2 ballCenter = ball.getPosition() + ball.getRadius();

		glm::vec2 difference = ballCenter - aabbCenter;

		glm::vec2 clamped = glm::clamp(difference, -aabbHalf, aabbHalf);

		glm::vec2 closestPoint = aabbCenter + clamped;

		difference = closestPoint - ballCenter;
		if (glm::length(difference) < ball.getRadius()) {
			return std::tuple(true, _getCollisionDirection(difference), difference);
		}
		return NoneCollision;
	}
}