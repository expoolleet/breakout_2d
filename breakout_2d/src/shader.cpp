#include "pch.hpp"

#include "glad/glad.h"
#include "logging.hpp"
#include "shader.hpp"
#include "shader_manager.hpp"
#include "shader_observer.hpp"
#include "string_operators.hpp"

#include <format>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <string_view>
#include <type_traits>
#include <variant>
#include <unordered_set>

Shader::Shader(std::string vsPath, std::string fsPath) {
	ShaderManager::compileProgram(m_ID, vsPath, fsPath);
	ShaderManager::saveShader(m_ID, *this);
	ShaderObserver::Get().registerShader(m_ID, { vsPath, fsPath });
}

Shader::Shader(std::string vsPath, std::string fsPath, std::string gsPath) {
	ShaderManager::compileProgram(m_ID, vsPath, gsPath, fsPath);
	ShaderManager::saveShader(m_ID, *this);
	ShaderObserver::Get().registerShader(m_ID, { vsPath, gsPath, fsPath });
}

int Shader::_getUniformLocation(std::string_view name) {
	int location = glGetUniformLocation(m_ID, name.data());
	if (location == -1) {
		static std::unordered_set<std::string, string_hash, string_view_equal> warningUniforms;
		if (!warningUniforms.contains(name)) {
			warningUniforms.insert(std::string { name });
			Logging::Warn(std::format("Invalid uniform name: {}", name));
		}
	}
	return location;
}

Shader::~Shader() {
	clear();
}

void Shader::use() const {
	glUseProgram(m_ID);
}

void Shader::clear() {
	if (m_ID == 0) return;
	int activeShaderID;
	glGetIntegerv(GL_CURRENT_PROGRAM, &activeShaderID);
	if (static_cast<unsigned int>(activeShaderID) == m_ID)
		glUseProgram(0);
	glDeleteProgram(m_ID);
	m_ID = 0;
}

unsigned int Shader::getID() const {
	return m_ID;
}

void Shader::setModelViewProjection(glm::mat4 &model, glm::mat4 &view, glm::mat4 &projection) {
	setMat4("model", model);
	setMat4("view", view);
	setMat4("projection", projection);
}

void Shader::setViewProjection(glm::mat4 &view, glm::mat4 &projection) {
	setMat4("view", view);
	setMat4("projection", projection);
}

void Shader::bindTextureUnit(unsigned int unit, unsigned int textureId) {
	glBindTextureUnit(unit, textureId);
}

void Shader::setMat4(std::string_view name, const glm::mat4 *mat, int count) {
	glProgramUniformMatrix4fv(m_ID, _getUniformLocation(name), count, GL_FALSE, glm::value_ptr(mat[0]));
}

void Shader::setMat4(std::string_view name, const float *mat) {
	glProgramUniformMatrix4fv(m_ID, _getUniformLocation(name), 1, GL_FALSE, mat);
}

void Shader::setMat4(std::string_view name, const glm::mat4 &mat) {
	saveValue(name, mat);
	glProgramUniformMatrix4fv(m_ID, _getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::setMat3(std::string_view name, const glm::mat3 &mat) {
	saveValue(name, mat);
	glProgramUniformMatrix3fv(m_ID, _getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::setMat3(std::string_view name, const float *mat) {
	glProgramUniformMatrix3fv(m_ID, _getUniformLocation(name), 1, GL_FALSE, mat);
}

void Shader::setVeñ4(std::string_view name, float x, float y, float z, float w) {
	saveValue(name, glm::vec4(x, y, z, w));
	glProgramUniform4f(m_ID, _getUniformLocation(name), x, y, z, w);
}

void Shader::setVeñ4(std::string_view name, float *vec) {
	glProgramUniform4fv(m_ID, _getUniformLocation(name), 1, vec);
}

void Shader::setVeñ4(std::string_view name, const glm::vec4 &vec) {
	saveValue(name, vec);
	glProgramUniform4fv(m_ID, _getUniformLocation(name), 1, glm::value_ptr(vec));
}

void Shader::setVec3(std::string_view name, float x, float y, float z) {
	saveValue(name, glm::vec3(x, y, z));
	glProgramUniform3f(m_ID, _getUniformLocation(name), x, y, z);
}

void Shader::setVec2(std::string_view name, const glm::vec2 &vec) {
	saveValue(name, vec);
	glProgramUniform2fv(m_ID, _getUniformLocation(name), 1, glm::value_ptr(vec));
}

void Shader::setVec3(std::string_view name, const float *vec) {	
	glProgramUniform3fv(m_ID, _getUniformLocation(name), 1, vec);
}

void Shader::setVec3(std::string_view name, const glm::vec3 &vec) {
	saveValue(name, vec);
	glProgramUniform3fv(m_ID, _getUniformLocation(name), 1, glm::value_ptr(vec));
}

void Shader::setVec3(std::string_view name, const glm::vec3 *vec, int count) {
	glProgramUniform3fv(m_ID, _getUniformLocation(name), count, glm::value_ptr(vec[0]));
}

void Shader::setFloat(std::string_view name, float value) {
	saveValue(name, value);
	glProgramUniform1f(m_ID, _getUniformLocation(name), value);
}

void Shader::setBool(std::string_view name, bool value) {
	saveValue(name, value);
	glProgramUniform1i(m_ID, _getUniformLocation(name), value);
}

void Shader::setInt(std::string_view name, int value) {
	saveValue(name, value);
	glProgramUniform1i(m_ID, _getUniformLocation(name), value);
}

void Shader::resetUniforms() {
	for (const auto &[name, value] : m_uniformValues) {
		int location = glGetUniformLocation(m_ID, name.data());
		if (location == -1)
			continue;

		std::visit([&](auto &&arg) {
			using T = std::decay_t<decltype(arg)>;
			if constexpr (std::is_same_v<T, int> || std::is_same_v<T, bool>) glProgramUniform1i(m_ID, location, arg);
			else if constexpr (std::is_same_v<T, float>) glProgramUniform1f(m_ID, location, arg);
			else if constexpr (std::is_same_v<T, glm::vec4>) glProgramUniform4fv(m_ID, location, 1, glm::value_ptr(arg));
			else if constexpr (std::is_same_v<T, glm::vec3>) glProgramUniform3fv(m_ID, location, 1, glm::value_ptr(arg));
			else if constexpr (std::is_same_v<T, glm::vec2>) glProgramUniform2fv(m_ID, location, 1, glm::value_ptr(arg));
			else if constexpr (std::is_same_v<T, glm::mat4>) glProgramUniformMatrix4fv(m_ID, location, 1, GL_FALSE, glm::value_ptr(arg));
			else if constexpr (std::is_same_v<T, glm::mat3>) glProgramUniformMatrix3fv(m_ID, location, 1, GL_FALSE, glm::value_ptr(arg));
		}, value);
	}
}

template <typename T> void Shader::saveValue(std::string_view name, T value) {
	auto it = m_uniformValues.find(name);
	if (it != m_uniformValues.end()) {
		it->second = value;
		return;
	}
	std::string key { name };
	m_uniformValues[key] = value;	
}

template void Shader::saveValue<float>(std::string_view, float);
template void Shader::saveValue<int>(std::string_view, int);
template void Shader::saveValue<bool>(std::string_view, bool);
template void Shader::saveValue<glm::vec4>(std::string_view, glm::vec4);
template void Shader::saveValue<glm::vec3>(std::string_view, glm::vec3);
template void Shader::saveValue<glm::vec2>(std::string_view, glm::vec2);
template void Shader::saveValue<glm::mat4>(std::string_view, glm::mat4);
template void Shader::saveValue<glm::mat3>(std::string_view, glm::mat3);
