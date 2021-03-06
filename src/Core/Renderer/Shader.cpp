#include "Core/Renderer/Shader.h"
#include <GL/glew.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

/* auxiliar struct for storing the vertex and fragment shader code */

struct ShaderCode
{
	std::string vertexShaderCode;
	std::string fragmentShaderCode;
};

/* parse the shader */

static ShaderCode ParseShader(const std::string& path)
{
	enum class ShaderType
	{
		VERTEX,
		FRAGMENT
	};

	std::ifstream file(path);
	std::stringstream ss[2];

	if (file.is_open())
	{
		std::string line;
		ShaderType type = ShaderType::VERTEX;

		while (std::getline(file, line))
		{
			if (line.find("#type") != std::string::npos)
			{
				if (line.find("vertex") != std::string::npos)
					type = ShaderType::VERTEX;
				else if (line.find("fragment") != std::string::npos)
					type = ShaderType::FRAGMENT;
			}
			else
				ss[(int)type] << line << "\n";
		}
	}

	return { ss[0].str(), ss[1].str() };
}

/* compile shader */

static unsigned int CompileShader(const std::string& code, int type)
{
	unsigned int shaderId = glCreateShader(type);

	const char* cstrCode = code.c_str();

	glShaderSource(shaderId, 1, &cstrCode, nullptr);
	glCompileShader(shaderId);

	int compileStatus;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileStatus);

	if (compileStatus == GL_FALSE)
	{
		int length;
		glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &length);
		char* message = new char[length];
		glGetShaderInfoLog(shaderId, length, &length, message);

		std::cout << message << std::endl;

		delete[] message;

		glDeleteShader(shaderId);

		return 0;
	}

	return shaderId;
}

/* SHADER CLASS */

Shader::Shader()
{
	m_id = 0;
}

Shader::Shader(Shader&& other) noexcept
{
	m_id = other.m_id;
	m_path = std::move(other.m_path);
	m_uniformCache = std::move(other.m_uniformCache);

	other.m_id = 0;
}

Shader::Shader(const std::string& path)
{
	Load(path);
}

Shader::~Shader()
{
	glDeleteProgram(m_id);
	 
	std::cout << "[INFO] Shader destroyed \"" << m_path << "\"" << std::endl;
}

void Shader::Load(const std::string& path)
{
	// parse

	auto[vertexShaderCode, fragmentShaderCode] = ParseShader(path);

	// check if the parse is correct

	if (vertexShaderCode.size() == 0 || fragmentShaderCode.size() == 0)
	{
		std::cout << "[ERROR] Shader loading \"" << path << "\"" << std::endl;
		return;
	}

	// compile the vertex and fragment shader

	unsigned int vertexShaderId = CompileShader(vertexShaderCode, GL_VERTEX_SHADER);
	unsigned int fragmentShaderId = CompileShader(fragmentShaderCode, GL_FRAGMENT_SHADER);

	// check if the compilation is correct

	if (vertexShaderId == 0 || fragmentShaderId == 0)
	{
		std::cout << "[ERROR] Shader compilation \"" << path << "\"" << std::endl;
		return;
	}

	// if all correct then create the shader program and attach the shaders

	m_id = glCreateProgram();
	m_path = path;

	glAttachShader(m_id, vertexShaderId);
	glAttachShader(m_id, fragmentShaderId);
	glLinkProgram(m_id);
	glValidateProgram(m_id);

	glDeleteShader(vertexShaderId);
	glDeleteShader(fragmentShaderId);

	// shader corretly

	std::cout << "[INFO] Shader loaded \"" << path << "\"" << std::endl;
}

int Shader::GetUniformLocation(const std::string& name)
{
	// check if the uniform is already in the "cache"

	auto it = m_uniformCache.find(name);

	if (it != m_uniformCache.end())
		return it->second;

	// if not get the location

	int location = glGetUniformLocation(m_id, name.c_str());

	// check if doesnt exist to output a warning

	if (location == -1)
		std::cout << "[WARNING] Uniform \"" << name << "\" does not exist" << std::endl;

	// even if it does not exist store the uniform into the hashmap

	m_uniformCache[name] = location;

	return location;
}

void Shader::Bind() const
{
	glUseProgram(m_id);
}

void Shader::UnBind() const
{
	glUseProgram(0);
}

void Shader::SetUniformMat4(const std::string& name, const glm::mat4& mat)
{
	int uniformId = GetUniformLocation(name);

	if (uniformId != -1)
		glUniformMatrix4fv(uniformId, 1, false, &mat[0][0]);
}

void Shader::SetUniform1iv(const std::string& name, int count, const int* data)
{
	int uniformId = GetUniformLocation(name);

	if (uniformId != -1)
		glUniform1iv(uniformId, count, data);
}

void Shader::SetUniform1i(const std::string& name, int data)
{
	int uniformId = GetUniformLocation(name);

	if (uniformId != -1)
		glUniform1i(uniformId, data);
}

void Shader::SetUniform1f(const std::string& name, float data)
{
	int uniformId = GetUniformLocation(name);

	if (uniformId != -1)
		glUniform1f(uniformId, data);
}

void Shader::SetUniformVec2(const std::string& name, const glm::vec2& data)
{
	int uniformId = GetUniformLocation(name);

	if (uniformId != -1)
		glUniform2f(uniformId, data.x, data.y);
}

void Shader::SetUniformVec3(const std::string& name, const glm::vec3& data)
{
	int uniformId = GetUniformLocation(name);

	if (uniformId != -1)
		glUniform3f(uniformId, data.x, data.y, data.z);
}

void Shader::SetUniformVec4(const std::string& name, const glm::vec4& data)
{
	int uniformId = GetUniformLocation(name);

	if (uniformId != -1)
		glUniform4f(uniformId, data.x, data.y, data.z, data.w);
}

void Shader::SetUniformVec2v(const std::string& name, int count, const glm::vec2* data)
{
	int uniformId = GetUniformLocation(name);

	if (uniformId != -1)
		glUniform2fv(uniformId, count, (const float*)data);
}

void Shader::SetUniformVec3v(const std::string& name, int count, const glm::vec3* data)
{
	int uniformId = GetUniformLocation(name);

	if (uniformId != -1)
		glUniform3fv(uniformId, count, (const float*)data);
}

Shader& Shader::operator=(Shader&& other) noexcept
{
	if (this != &other)
	{
		m_id = other.m_id;
		m_path = std::move(other.m_path);
		m_uniformCache = std::move(other.m_uniformCache);

		other.m_id = 0;
	}

	return *this;
}