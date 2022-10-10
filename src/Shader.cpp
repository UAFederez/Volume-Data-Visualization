#include "Shader.h"
#include <wx/wx.h>

Shader::Shader(const std::string& vsPath, const std::string& fsPath):
	vertSourcePath(vsPath),
	fragSourcePath(fsPath)
{
	CompileAndLinkProgram();
}

void Shader::UseProgram() {
	glUseProgram(programId);
}

void Shader::SetMatrix4x4(const char* uniformName, const glm::mat4& mat)
{
	glUniformMatrix4fv(glGetUniformLocation(programId, uniformName), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::SetVector3(const char* uniformName, const glm::vec3& vec)
{
	glUniform3fv(glGetUniformLocation(programId, uniformName), 1, glm::value_ptr(vec));
}

void Shader::SetVector2(const char* uniformName, float f1, float f2)
{
	glUniform2f(glGetUniformLocation(programId, uniformName), f1, f2);
}

void Shader::SetFloat(const char* uniformName, const float f)
{
	glUniform1f(glGetUniformLocation(programId, uniformName), f);
}

void Shader::CompileAndLinkProgram()
{
	const std::string vertSource = LoadShaderSource(vertSourcePath);
	const std::string fragSource = LoadShaderSource(fragSourcePath);

	const char* vertSourceCStr = vertSource.c_str();
	const char* fragSourceCStr = fragSource.c_str();

	GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource (vertShader, 1, &vertSourceCStr, NULL);
	glCompileShader(vertShader);

	if (!CheckIfCompileOrLinkSuccessful(vertShader, vertSourcePath))
	{
		std::cerr << "Vertex shader compilation failed!";
		return;
	}


	GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource (fragShader, 1, &fragSourceCStr, NULL);
	glCompileShader(fragShader);

	if (!CheckIfCompileOrLinkSuccessful(fragShader, fragSourcePath))
	{
		std::cerr << "Fragment shader compilation failed!";
		hasError = 2;
		return;
	}

	programId = glCreateProgram();
	glAttachShader(programId, vertShader);
	glAttachShader(programId, fragShader);
	glLinkProgram(programId);

	glDeleteShader(vertShader);
	glDeleteShader(fragShader);

	if (!CheckIfCompileOrLinkSuccessful(programId))
	{
		std::cerr << "Program linking failed!";
		hasError = 3;
		return;
	}
}


bool Shader::CheckIfCompileOrLinkSuccessful(GLuint id, const std::string& shaderPath)
{
	int success;
	char infoLog[512];

	if (shaderPath.empty())	// id is for a program
	{
		glGetProgramiv(id, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(id, 512, NULL, infoLog);
			wxLogDebug("Program link: %s\n", infoLog);
			return false;
		}
	}
	else // id is for a shader
	{
		glGetShaderiv(id, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(id, 512, NULL, infoLog);
			wxLogDebug("In \'%s\': %s\n", shaderPath.c_str(), infoLog);
			return false;
		}
	}
	return true;
}

std::string Shader::LoadShaderSource(const std::string& path)
{
	std::ifstream file(path);
	std::ostringstream oss;

	oss << file.rdbuf();
	return oss.str();
}