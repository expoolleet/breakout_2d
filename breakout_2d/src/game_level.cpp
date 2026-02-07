#include "pch.hpp"

#include "brick.hpp"
#include "brick_type.hpp"
#include "game_level.hpp"
#include "logging.hpp"
#include "texture_manager.hpp"
#include "window.hpp"

#include <algorithm>
#include <format>
#include <fstream>
#include <glm/glm.hpp>
#include <ios>
#include <sstream>
#include <string>
#include <vector>

GameLevel::GameLevel(std::string levelPath, int width, int height) : m_width(width), m_height(height) {
	m_bricks.clear();
	try {
		std::string line;
		std::ifstream fileStream(levelPath);

		if (!fileStream.is_open()) {
			Logging::Error(std::format("Could not load the level file: {}", levelPath));
			return;
		}
		int tileCode;
		if (fileStream) {
			while (std::getline(fileStream, line)) {
				std::istringstream stringStream(line);
				std::vector<int> row;
				while (stringStream >> tileCode) {
					row.push_back(tileCode);
				}
				m_tileCodes.push_back(row);
			}
		}
	}
	catch (std::ifstream::failure e) {
		Logging::Error(e.what());
	}
}

GameLevel::~GameLevel() {
	m_bricks.clear();
}

void GameLevel::load() {
	if (m_isLoaded)
		return;
	m_isLoaded = true;

	int height = m_tileCodes.size();
	int width = m_tileCodes[0].size();

	float offset = 3.0f;
	float unitWidth =  m_width / (static_cast<float>(width));
	float unitHeight = m_height / height;
	for (int i = 0; i < height; ++i) {
		for (int j = 0; j < width; ++j) {
			int code = m_tileCodes[i][j];
			if (code == 0)
				continue;
			
			glm::vec2 position = glm::vec2(j * unitWidth + offset / 2, Window::getHeight() - (unitHeight + i * unitHeight + offset));
			glm::vec2 size = glm::vec2(unitWidth - offset, unitHeight - offset);

			switch (code) {
				case -1:
					m_bricks.push_back(Brick(TextureManager::getTexture(UNDESTROYABLE_BRICK), position, size, BrickType::UNDESTROYABLE)); break;
				case 1:
					m_bricks.push_back(Brick(TextureManager::getTexture(STANDARD_BRICK), position, size, BrickType::STANDARD)); break;
				case 2:
					m_bricks.push_back(Brick(TextureManager::getTexture(STANDARD_BRICK), position, size, BrickType::HARD)); break;
				case 3:
					m_bricks.push_back(Brick(TextureManager::getTexture(STANDARD_BRICK), position, size, BrickType::EXTREMELY_TOUGH)); break;
			}
			
		}
	}
	std::partition(m_bricks.begin(), m_bricks.end(), [](auto &brick) { return brick.isDestroyable(); });
}

void GameLevel::restart() {
	for (auto &brick : m_bricks) {
		if (!brick.isDestroyable())
			continue;
		brick.reset();
	}
}

std::vector<Brick> &GameLevel::getBricks() {
	return m_bricks;
}

bool GameLevel::isFinished() {
	for (auto &brick : m_bricks) {
		if (!brick.isDestroyable())
			continue;
		if (brick.getCurrentHardnessPoints() > 0)
			return false;
	}
	return true;
}

bool GameLevel::isLoaded() {
	return m_isLoaded;
}
