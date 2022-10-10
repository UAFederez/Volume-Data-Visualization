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
	void UseProgram();
	void SetMatrix4x4(const char* uniformName, const glm::mat4& mat);
	void SetVector3(const char* uniformName, const glm::vec3& vec);
	void SetVector2(const char* uniformName, float f1, float f2);
	void SetFloat(const char* uniformName, const float f);

	int hasError = 0;
private:
	void CompileAndLinkProgram();
	bool CheckIfCompileOrLinkSuccessful(GLuint id, const std::string& shaderPath = "");
	std::string LoadShaderSource(const std::string& path);

	GLuint programId;
	std::string vertSourcePath;
	std::string fragSourcePath;
};
