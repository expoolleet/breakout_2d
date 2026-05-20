#pragma once

#include <glm/glm.hpp>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>

#include "string_operators.hpp"

// max texture units (slots) is 32
#define SKY_BOX_SLOT 10
#define SHADOW_MAP_SLOT 11
#define SHADOW_CUBE_SLOT 12
#define IRRADIANCE_MAP_SLOT 13
#define PREFILTER_MAP_SLOT 14
#define BRDF_LUT_SLOT 15

#define G_BUFFER_DEPTH_SLOT 20

using UniformValue = std::variant<int, bool, float, glm::vec2, glm::vec3, glm::vec4, glm::mat3, glm::mat4>;

class Shader {
   private:
    unsigned int m_ID = 0;
    std::unordered_map<std::string, UniformValue, string_hash, string_view_equal> m_uniformValues;
    int _getUniformLocation(std::string_view name);

    Shader(const Shader &) = delete;  // delete copy constructor
   public:
    Shader() = default;
    ~Shader();
    Shader(std::string vsPath, std::string fsPath);
    Shader(std::string vsPath, std::string fsPath, std::string gsPath);

    void use() const;
    void clear();
    unsigned int getID() const noexcept;
    void setModelViewProjection(glm::mat4 &model, glm::mat4 &view, glm::mat4 &projection);
    void setViewProjection(glm::mat4 &view, glm::mat4 &projection);
    void bindTextureUnit(unsigned int unit, unsigned int textureId);
    void setMat4(std::string_view name, const glm::mat4 *mat, int count);
    void setMat4(std::string_view name, const float *mat);
    void setMat4(std::string_view name, const glm::mat4 &mat);
    void setMat3(std::string_view name, const glm::mat3 &mat);
    void setMat3(std::string_view name, const float *mat);
    void setVec4(std::string_view name, float x, float y, float z, float w);
    void setVec4(std::string_view name, float *vec);
    void setVec4(std::string_view name, const glm::vec4 &vec);
    void setVec3(std::string_view name, const float *vec);
    void setVec3(std::string_view name, const glm::vec3 &vec);
    void setVec3(std::string_view name, const glm::vec3 *vec, int count);
    void setVec3(std::string_view name, float x, float y, float z);
    void setVec2(std::string_view name, const glm::vec2 &vec);
    void setFloat(std::string_view name, float value);
    void setBool(std::string_view name, bool value);
    void setInt(std::string_view name, int value);
    void resetUniforms();

    template <typename T>
    void saveValue(std::string_view name, T value);
};

using ShaderSPtr = std::shared_ptr<Shader>;
using ShaderUPtr = std::unique_ptr<Shader>;
using ShaderView = Shader *;
