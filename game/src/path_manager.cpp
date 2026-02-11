#include "path_manager.hpp"

#include <filesystem>
#include <string>
#include <string_view>

namespace fs = std::filesystem;

void PathManager::init() {
	fs::path currentPath = fs::current_path();
	int maxLevel = 5;
	for (int i = 0; i < maxLevel; ++i) {
		if (fs::exists(currentPath / RESOURCES_DIR)) {
			m_rootPath = currentPath;
			return;
		}
		currentPath = currentPath.parent_path();
	}
}

std::string PathManager::getRootPath() {
	return m_rootPath.string();
}

std::string PathManager::getResourcePath(std::string_view relativePath) {
	return (m_rootPath / RESOURCES_DIR / relativePath).string();
}
