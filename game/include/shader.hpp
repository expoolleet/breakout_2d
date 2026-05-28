#pragma once

#include <glm/glm.hpp>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>

#include "observer_ptr.hpp"
#include "shader_observer.hpp"
#include "string_operators.hpp"

using UniformValue = std::variant<int, bool, float, glm::vec2, glm::vec3, glm::vec4, glm::mat3, glm::mat4>;

class Shader {
   private:
    unsigned int m_ID = 0;
    std::unordered_map<std::string, UniformValue, string_hash, string_view_equal> m_uniformValues;
    int _getUniformLocation(std::string_view name);

   public:
    Shader() = default;
    ~Shader() noexcept;
    Shader(std::string vsPath, std::string fsPath, ShaderObserverPtr observer);
    Shader(std::string vsPath, std::string fsPath, std::string gsPath, ShaderObserverPtr observer);

    Shader(const Shader &) = delete;
    Shader &operator=(const Shader &) = delete;

    bool operator==(const Shader &other) const noexcept;
    bool operator!=(const Shader &other) const noexcept;

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
    void setBool(std::string_view name, bool value);
    void setFloat(std::string_view name, float value);
    void setInt(std::string_view name, int value);
    void resetUniforms();

    template <typename T>
    void saveValue(std::string_view name, T value);
};

using ShaderPtr = observer_ptr<Shader>;
