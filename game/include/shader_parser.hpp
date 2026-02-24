#pragma once

#include "glad/glad.h"
#include "logging.hpp"

#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_set>

namespace ShaderParser {
inline static std::unordered_set<std::string> includeFormats = {".glsl"};

inline void parseIncludesCPU(std::string &includes, std::string &sourceCode) {
    std::string extLine = "#extension GL_ARB_shading_language_include : require";
    for (const auto &entry : std::filesystem::directory_iterator(includes)) {
        if (!includeFormats.count(entry.path().extension().string()))
            continue;

        std::ifstream readStream;
        std::string source;

        try {
            readStream.open(entry.path());
            std::stringstream stream;
            stream << readStream.rdbuf();
            source = stream.str();
            readStream.close();
        } catch (std::ifstream::failure e) {
            logging::Error("Includes initialization: file was not successfully read\n{}", entry.path().string());
        }

        std::string includeName = "/" + entry.path().filename().string();
        std::string include = "#include \"" + includeName + "\"";
        size_t pos = sourceCode.find(include.c_str());
        if (pos != std::string::npos) {
            size_t size = include.size();
            sourceCode.replace(pos, size, source);
            parseIncludesCPU(includes, sourceCode);
        }
        size_t extLinePos = sourceCode.find(extLine);
        while (extLinePos != std::string::npos) {
            sourceCode.replace(extLinePos, extLine.size(), "");
            extLinePos = sourceCode.find(extLine, extLinePos + extLine.size());
        }
    }
}

inline void parseIncludesGPU(std::string &includes) {
    for (const auto &entry : std::filesystem::directory_iterator(includes)) {

        if (!includeFormats.count(entry.path().extension().string()))
            continue;

        std::ifstream readStream;
        std::string source;

        try {
            readStream.open(entry.path());
            std::stringstream stream;
            stream << readStream.rdbuf();
            source = stream.str();
            readStream.close();
        } catch (std::ifstream::failure e) {
            std::cout << "ERROR::INIT_INCLUDES::FILE_NOT_SUCCESSFULLY_READ\n" << entry.path() << std::endl;
        }

        std::string name = "/" + entry.path().filename().string();

        glNamedStringARB(GL_SHADER_INCLUDE_ARB, static_cast<unsigned int>(name.size()), name.c_str(),
                         static_cast<unsigned int>(source.size()), source.c_str());
    }
}
} // namespace ShaderParser
