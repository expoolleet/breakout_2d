#pragma once

#include <glad/glad.h>

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "shader.hpp"

inline constexpr int SHADER_INFO_LOG_SIZE = 2048;

#define SHADING_LANGUAGE_INCLUDE_SUPPORT (GLAD_GL_ARB_shading_language_include && glNamedStringARB != NULL)

class ShaderManager {
   private:
    static std::unordered_map<unsigned int *, Shader &> m_shaders;
    static std::string m_pathToIncludes;
    static void _deleteShaders(std::vector<unsigned int> &shaders);
    static bool _linkShaders(unsigned int &programID, std::vector<unsigned int> &shaders);
    static bool _readShaderFile(std::string &sSource, std::string_view sPath);
    static bool _compileShader(unsigned int &shaderID, unsigned int shaderType, const char **source);
    ShaderManager() = delete;                                  // delete constructor
    ShaderManager(const ShaderManager &) = delete;             // delete copy constructor
    ShaderManager &operator=(const ShaderManager &) = delete;  // delete assign operator

   public:
    static void compileProgram(unsigned int &programID, std::string_view vsPath, std::string_view fsPath);
    static void compileProgram(unsigned int &programID, std::string_view vsPath, std::string_view gsPath, std::string_view fsPath);
    static void saveShader(unsigned int &programID, Shader &shader);
    static void reloadShader(unsigned int &programID, std::vector<std::string> &shaderPaths);
    static void initIncludes(std::string pathToIncludes);
    static void checkIncludes(std::string &shaderCode);
    static void printSavedShaders();
};
