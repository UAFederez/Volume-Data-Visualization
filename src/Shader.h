#pragma once

#include <GL/glew.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader {

public:
	Shader() = default;
	Shader(const std::string& vsPath, const std::string& fsPath);

	Shader(const Shader&)			 = delete;
	Shader& operator=(const Shader&) = delete;

	Shader(Shader&& s) noexcept;
	Shader& operator=(Shader&& s) noexcept;

	void UseProgram();
	void SetMatrix4x4(const char* uniformName, const glm::mat4& mat);
	void SetMatrix3x3(const char* uniformName, const glm::mat3& mat);
	void SetVector3(const char* uniformName, const glm::vec3& vec);
	void SetVector2(const char* uniformName, float f1, float f2);
	void SetFloat(const char* uniformName, const float f);
	void SetUint(const char* uniformName, const uint32_t i);
	void SetInt(const char* uniformName, const int32_t i);
	~Shader();
private:
	void CompileAndLinkProgram();
	bool CheckIfCompileOrLinkSuccessful(GLuint id, const std::string& shaderPath = "");
	std::string LoadShaderSource(const std::string& path);

	bool   m_isValid = false;
	GLuint m_programId;
	std::string m_vertSourcePath;
	std::string m_fragSourcePath;
};
