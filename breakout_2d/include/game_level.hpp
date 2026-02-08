#pragma once

#include "brick.hpp"

#include <string>
#include <vector>

class GameLevel {
private:
	std::vector<Brick> m_bricks = { };
	std::vector<std::vector<int>> m_tileCodes = { };
	int m_width = 0;
	int m_height = 0;
	glm::vec2 m_offset = glm::vec2(0.0f);
	bool m_isLoaded = false;
public:
	static GameLevel DefaultLevel;

	GameLevel() = default;
	GameLevel(std::string levelPath, int width, int height, glm::vec2 offset = glm::vec2(0.0f));
	~GameLevel();

	void load();
	void restart();
	std::vector<Brick> &getBricks();
	bool isFinished();
	bool isLoaded();
};