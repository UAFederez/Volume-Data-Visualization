#include "Shader.h"

Shader::Shader(const std::string& vsPath, const std::string& fsPath):
    m_vertSourcePath(vsPath),
    m_fragSourcePath(fsPath)
{
    CompileAndLinkProgram();
    m_isValid = true;
}

Shader::Shader(Shader&& shader) noexcept:
    m_isValid  (std::move(shader.m_isValid)),
    m_programId(std::move(shader.m_programId)),
    m_vertSourcePath(std::move(shader.m_vertSourcePath)),
    m_fragSourcePath(std::move(shader.m_fragSourcePath))
{
    shader.m_programId = 0;
    shader.m_isValid   = false;
}

Shader& Shader::operator=(Shader&& shader) noexcept
{
    m_isValid		 = std::move(shader.m_isValid);
    m_programId	     = std::move(shader.m_programId);
    m_vertSourcePath = std::move(shader.m_vertSourcePath);
    m_fragSourcePath = std::move(shader.m_fragSourcePath);

    shader.m_isValid   = false;
    shader.m_programId = 0;

    return *this;
}

void Shader::UseProgram() {
    glUseProgram(m_programId);
}

void Shader::SetMatrix4x4(const char* uniformName, const glm::mat4& mat)
{
    glUniformMatrix4fv(glGetUniformLocation(m_programId, uniformName), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::SetMatrix3x3(const char* uniformName, const glm::mat3& mat)
{
    glUniformMatrix3fv(glGetUniformLocation(m_programId, uniformName), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::SetVector3(const char* uniformName, const glm::vec3& vec)
{
    glUniform3fv(glGetUniformLocation(m_programId, uniformName), 1, glm::value_ptr(vec));
}

void Shader::SetVector2(const char* uniformName, float f1, float f2)
{
    glUniform2f(glGetUniformLocation(m_programId, uniformName), f1, f2);
}

void Shader::SetFloat(const char* uniformName, const float f)
{
    glUniform1f(glGetUniformLocation(m_programId, uniformName), f);
}

void Shader::SetInt(const char* uniformName, const int32_t i)
{
    glUniform1i(glGetUniformLocation(m_programId, uniformName), i);
}

void Shader::SetUint(const char* uniformName, const uint32_t i)
{
    glUniform1ui(glGetUniformLocation(m_programId, uniformName), i);
}

void Shader::CompileAndLinkProgram()
{
    const std::string vertSource = LoadShaderSource(m_vertSourcePath);
    const std::string fragSource = LoadShaderSource(m_fragSourcePath);

    const char* vertSourceCStr = vertSource.c_str();
    const char* fragSourceCStr = fragSource.c_str();

    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource (vertShader, 1, &vertSourceCStr, NULL);
    glCompileShader(vertShader);

    if (!CheckIfCompileOrLinkSuccessful(vertShader, m_vertSourcePath))
    {
        std::cerr << "Vertex shader compilation failed!";
        return;
    }

    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource (fragShader, 1, &fragSourceCStr, NULL);
    glCompileShader(fragShader);

    if (!CheckIfCompileOrLinkSuccessful(fragShader, m_fragSourcePath))
    {
        std::cerr << "Fragment shader compilation failed!";
        return;
    }

    m_programId = glCreateProgram();
    glAttachShader(m_programId, vertShader);
    glAttachShader(m_programId, fragShader);
    glLinkProgram(m_programId);

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    if (!CheckIfCompileOrLinkSuccessful(m_programId))
    {
        std::cerr << "Program linking failed!";
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
            std::printf("Program link: %s\n", infoLog);
            return false;
        }
    }
    else // id is for a shader
    {
        glGetShaderiv(id, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(id, 512, NULL, infoLog);
            std::printf("In \'%s\': %s\n", shaderPath.c_str(), infoLog);
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

Shader::~Shader()
{
    if (m_isValid)
    {
        glDeleteProgram(m_programId);
    }
}
