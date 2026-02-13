#pragma once

#include <filesystem>
#include <string>
#include <string_view>

#define RESOURCES_DIR "resources"

namespace fs = std::filesystem;

class PathManager {
    inline static fs::path m_rootPath = "";

  public:
    static void init();
    static std::string getRootPath();
    static std::string getResourcePath(std::string_view relativePath);
};
