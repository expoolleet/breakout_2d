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
	bool m_isLoaded = false;
public:
	static GameLevel DefaultLevel;

	GameLevel() = default;
	GameLevel(std::string levelPath, int width, int height);
	~GameLevel();

	void load();
	void restart();
	std::vector<Brick> &getBricks();
	bool isFinished();
	bool isLoaded();
};