#include "shader_manager.hpp"

#include <format>
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "glad/glad.h"
#include "logging.hpp"
#include "shader.hpp"
#include "shader_parser.hpp"

void ShaderManager::_deleteShaders(std::vector<unsigned int> &shaders) {
    for (unsigned int &shader : shaders) {
        glDeleteShader(shader);
    }
}

bool ShaderManager::_linkShaders(unsigned int &programID, std::vector<unsigned int> &shaders) {
    int success;
    programID = glCreateProgram();
    for (unsigned int &shader : shaders) {
        glAttachShader(programID, shader);
    }
    glLinkProgram(programID);
    glGetProgramiv(programID, GL_LINK_STATUS, &success);
    return success;
}

bool ShaderManager::_readShaderFile(std::string &sSource, std::string_view sPath) {
    std::ifstream sStream;
    sStream.exceptions(std::ifstream::badbit | std::ifstream::failbit);
    try {
        sStream.open(sPath.data());
        std::stringstream sStringStream;
        sStringStream << sStream.rdbuf();
        sStream.close();
        sSource = sStringStream.str();
    } catch (std::ifstream::failure e) {
        logging::Error("Error appeared when read the shader file ({}): {}", sPath, e.what());
        return false;
    }
    return true;
}

bool ShaderManager::_compileShader(unsigned int &shaderID, unsigned int shaderType, const char **source) {
    int success;
    shaderID = glCreateShader(shaderType);
    glShaderSource(shaderID, 1, source, NULL);
    glCompileShader(shaderID);
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);
    return success;
}

void ShaderManager::compileProgram(unsigned int &programID, std::string_view vsPath, std::string_view fsPath) {
    std::string vsSource;
    std::string fsSource;
    if (!_readShaderFile(vsSource, vsPath) || !_readShaderFile(fsSource, fsPath)) return;

    checkIncludes(vsSource);
    checkIncludes(fsSource);

    const char *vsCompileSource = vsSource.c_str();
    const char *fsCompileSource = fsSource.c_str();

    char log[SHADER_INFO_LOG_SIZE];

    unsigned int vertexShader = 0;
    if (!_compileShader(vertexShader, GL_VERTEX_SHADER, &vsCompileSource)) {
        glGetShaderInfoLog(vertexShader, SHADER_INFO_LOG_SIZE, NULL, log);
        logging::Error("Failed to compile the vertex shader:\n{}\n{}", vsPath, log);
        return;
    }

    unsigned int fragmentShader = 0;
    if (!_compileShader(fragmentShader, GL_FRAGMENT_SHADER, &fsCompileSource)) {
        glGetShaderInfoLog(fragmentShader, SHADER_INFO_LOG_SIZE, NULL, log);
        logging::Error("Failed to compile the vertex shader:\n{}\n{}", fsPath, log);
        return;
    }

    std::vector<unsigned int> shaders = {vertexShader, fragmentShader};
    if (!_linkShaders(programID, shaders)) {
        glGetProgramInfoLog(programID, SHADER_INFO_LOG_SIZE, NULL, log);
        logging::Error("Failed to link shaders:\n{}\n{}\n{}", vsPath, fsPath, log);
        return;
    }

    _deleteShaders(shaders);
}

void ShaderManager::compileProgram(unsigned int &programID, std::string_view vsPath, std::string_view gsPath, std::string_view fsPath) {
    std::string vsSource;
    std::string gsSource;
    std::string fsSource;
    if (!_readShaderFile(vsSource, vsPath) || !_readShaderFile(gsSource, gsPath) || !_readShaderFile(fsSource, fsPath)) return;

    checkIncludes(vsSource);
    checkIncludes(gsSource);
    checkIncludes(fsSource);

    const char *vsCompileSource = vsSource.c_str();
    const char *gsCompileSource = gsSource.c_str();
    const char *fsCompileSource = fsSource.c_str();

    char log[SHADER_INFO_LOG_SIZE];

    unsigned int vertexShader = 0;
    if (!_compileShader(vertexShader, GL_VERTEX_SHADER, &vsCompileSource)) {
        glGetShaderInfoLog(vertexShader, SHADER_INFO_LOG_SIZE, NULL, log);
        logging::Error("Failed to compile the vertex shader:\n{}\n{}", vsPath, log);
        return;
    }

    unsigned int geometryShader = 0;
    if (!_compileShader(geometryShader, GL_GEOMETRY_SHADER, &gsCompileSource)) {
        glGetShaderInfoLog(geometryShader, SHADER_INFO_LOG_SIZE, NULL, log);
        logging::Error("Failed to compile the geometry shader:\n{}\n{}", gsPath, log);
        return;
    }

    unsigned int fragmentShader = 0;
    if (!_compileShader(fragmentShader, GL_FRAGMENT_SHADER, &fsCompileSource)) {
        glGetShaderInfoLog(fragmentShader, SHADER_INFO_LOG_SIZE, NULL, log);
        logging::Error("Failed to compile the vertex shader:\n{}\n{}", fsPath, log);
        return;
    }

    std::vector<unsigned int> shaders = {vertexShader, geometryShader, fragmentShader};
    if (!_linkShaders(programID, shaders)) {
        glGetProgramInfoLog(programID, SHADER_INFO_LOG_SIZE, NULL, log);
        logging::Error("Failed to link shaders:\n{}\n{}\n{}\n{}", vsPath, gsPath, fsPath, log);
        return;
    }

    _deleteShaders(shaders);
}

void ShaderManager::saveShader(unsigned int &programID, Shader &shader) {
    if (m_shaders.contains(&programID)) {
        logging::Error("Can not save shader {} because it is already in container", programID);
        return;
    }
    m_shaders.insert({&programID, shader});
}

void ShaderManager::reloadShader(unsigned int &programID, std::vector<std::string> &shaderPaths) {
    unsigned int reloadedProgramID = 0;
    switch (shaderPaths.size()) {
        case 2:
            compileProgram(reloadedProgramID, shaderPaths[0], shaderPaths[1]);
            break;
        case 3:
            compileProgram(reloadedProgramID, shaderPaths[0], shaderPaths[1], shaderPaths[2]);
            break;
    };
    if (reloadedProgramID == 0) {
        logging::Log("An error occurred while reloading the shader with ID: {}", programID);
        return;
    }
    glDeleteProgram(programID);
    programID = reloadedProgramID;

    auto it = m_shaders.find(&programID);
    if (it != m_shaders.end()) {
        it->second.resetUniforms();
    }
}

void ShaderManager::initIncludes(std::string pathToIncludes) {
    m_pathToIncludes = pathToIncludes;
    if (!std::filesystem::exists(pathToIncludes))
        throw std::invalid_argument("Parsing shaders includes error: directory does not exists: " + pathToIncludes);
    if (!std::filesystem::is_directory(pathToIncludes))
        throw std::invalid_argument("Parsing shaders includes error: path leads to a file (not directory): " + pathToIncludes);
    if (!SHADING_LANGUAGE_INCLUDE_SUPPORT) {
        logging::Warn("'GL_ARB_shading_language_include' extension is not supported by the hardware, will do CPU parsing\n");
        return;
    }
    ShaderParser::parseIncludesGPU(m_pathToIncludes);
}

void ShaderManager::checkIncludes(std::string &shaderCode) {
    if (!SHADING_LANGUAGE_INCLUDE_SUPPORT && !m_pathToIncludes.empty()) {
        ShaderParser::parseIncludesCPU(m_pathToIncludes, shaderCode);
    }
}

void ShaderManager::printSavedShaders() {
    logging::Log("Printing available shaders:");
    int count = 0;
    for (auto &[id, shader] : m_shaders) {
        logging::LogSilent("{}. {}", ++count, *id);
    }
}

std::string ShaderManager::m_pathToIncludes = "";
std::unordered_map<unsigned int *, Shader &> ShaderManager::m_shaders = {};
