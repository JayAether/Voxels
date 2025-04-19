
#include <glad/glad.h>

#include "Shader.h"

#include <fstream>

struct ShaderString
{
	std::string vert;
	std::string frag;

	ShaderString(std::string vert, std::string frag)
	{
		this->vert = vert;
		this->frag = frag;
	}
};


static ShaderString parseShader(std::string path)
{

	enum ShaderType
	{
		NONE = -1,
		VERTEX,
		FRAGMENT,
		COUNT,
	};

	std::ifstream stream;

	//file.exceptions(std::ifstream::badbit | std::ifstream::failbit);

	std::string line;
	std::string ss[ShaderType::COUNT];

	ShaderType type = ShaderType::NONE;

	while (std::getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
			{
				type = ShaderType::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos)
			{
				type = ShaderType::FRAGMENT;
			}
		}
		else if (type != ShaderType::NONE)
			ss[(int)type] += line;
	}

	return ShaderString(ss[VERTEX], ss[FRAGMENT]);
}



Shader::Shader(const char* shaderPath)
{
	std::string sPath = shaderPath;
	ShaderString shaderSrc = parseShader(sPath);

	id = glCreateProgram();
	compileShader(GL_VERTEX_SHADER, shaderSrc.vert);
	compileShader(GL_FRAGMENT_SHADER, shaderSrc.frag);

	glLinkProgram(id);

	int result;
	glGetProgramiv(id, GL_LINK_STATUS, &result);
	if (result != 0)
	{
		int length;
		glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length);
		char* msg = (char*)alloca(length * sizeof(char));
		glGetProgramInfoLog(id, length, &length, msg);
		throw std::runtime_error(("failed to link program: ", msg));
	}
}

//Shader::Shader(std::string shaderPath):
//Shader::Shader(const char* vertPath, const char* fragPath);
//Shader::Shader(std::string vertPath, std::string fragPath);

Shader::~Shader()
{
	glDeleteProgram(id);
}

void Shader::useShader()
{
	glUseProgram(id);
}

//void Shader::setUniform(std::string uniformName, int val);		 // int
//void Shader::setUniform(const char* uniformName, int val);       // int
//void Shader::setUniform(std::string uniformName, float val);	 // float
//void Shader::setUniform(const char* uniformName, float val);	 // float
//
//void Shader::setUniform(std::string uniformName, glm::vec2 val); // vec2
//void Shader::setUniform(const char* uniformName, glm::vec2 val); // vec2
//void Shader::setUniform(std::string uniformName, glm::vec3 val); // vec3
//void Shader::setUniform(const char* uniformName, glm::vec3 val); // vec3
//void Shader::setUniform(std::string uniformName, glm::vec4 val); // vec4
//void Shader::setUniform(const char* uniformName, glm::vec4 val); // vec4
//
//void Shader::setUniform(std::string uniformName, glm::mat2 val);   // mat2
//void Shader::setUniform(const char* uniformName, glm::mat2 val);   // mat2
//void Shader::setUniform(std::string uniformName, glm::mat2x2 val); // mat2x2
//void Shader::setUniform(const char* uniformName, glm::mat2x2 val); // mat2x2
//void Shader::setUniform(std::string uniformName, glm::mat2x3 val); // mat2x3
//void Shader::setUniform(const char* uniformName, glm::mat2x3 val); // mat2x3
//void Shader::setUniform(std::string uniformName, glm::mat2x4 val); // mat2x4
//void Shader::setUniform(const char* uniformName, glm::mat2x4 val); // mat2x4
//
//void Shader::setUniform(std::string uniformName, glm::mat3 val);   // mat3
//void Shader::setUniform(const char* uniformName, glm::mat3 val);   // mat3
//void Shader::setUniform(std::string uniformName, glm::mat3x2 val); // mat3x2
//void Shader::setUniform(const char* uniformName, glm::mat3x2 val); // mat3x2
//void Shader::setUniform(std::string uniformName, glm::mat3x3 val); // mat3x3
//void Shader::setUniform(const char* uniformName, glm::mat3x3 val); // mat3x3
//void Shader::setUniform(std::string uniformName, glm::mat3x4 val); // mat3x4
//void Shader::setUniform(const char* uniformName, glm::mat3x4 val); // mat3x4
//
//void Shader::setUniform(std::string uniformName, glm::mat4 val);   // mat4
//void Shader::setUniform(const char* uniformName, glm::mat4 val);   // mat4
//void Shader::setUniform(std::string uniformName, glm::mat4x2 val); // mat4x2
//void Shader::setUniform(const char* uniformName, glm::mat4x2 val); // mat4x2
//void Shader::setUniform(std::string uniformName, glm::mat4x3 val); // mat4x3
//void Shader::setUniform(const char* uniformName, glm::mat4x3 val); // mat4x3
//void Shader::setUniform(std::string uniformName, glm::mat4x4 val); // mat4x4
//void Shader::setUniform(const char* uniformName, glm::mat4x4 val); // mat4x4



void Shader::compileShader(unsigned int type, std::string& source)
{
	assert(type == GL_VERTEX_SHADER || type == GL_FRAGMENT_SHADER);

	char infoLog[1024];

	unsigned int shaderID = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(type, 1, &src, nullptr);
	glCompileShader(shaderID);

	int result;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
	if (result != 0)
	{
		int length;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &length);
		char* msg = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(shaderID, length, &length, msg);
		throw std::runtime_error(("failed to compile shader: ", msg));
		glDeleteShader(shaderID);
		return;
	}
	
	glAttachShader(id, shaderID);
	glDeleteShader(shaderID);
}
