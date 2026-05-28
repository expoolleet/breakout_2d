#pragma once

#include <filesystem>
#include <string>
#include <string_view>

#include "observer_ptr.hpp"

namespace fs = std::filesystem;

class PathManager {
   private:
    fs::path m_rootPath;

   public:
    PathManager() = default;
    void init();
    std::string getRootPath();
    std::string getResourcePath(std::string_view relativePath);
};

using PathManagerPtr = observer_ptr<PathManager>;
