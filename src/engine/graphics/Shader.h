#pragma once

#include <glm/glm.hpp>

#include <string>

class Shader
{
public:
	Shader(const char* shaderPath);
	Shader(std::string shaderPath);
	Shader(const char* vertPath, const char* fragPath);
	Shader(std::string vertPath, std::string fragPath);
	~Shader();

	void useShader();

	void setUniform(std::string uniformName, int   val);	 // int
	void setUniform(const char* uniformName, int   val);     // int
	void setUniform(std::string uniformName, float val);	 // float
	void setUniform(const char* uniformName, float val);	 // float

	void setUniform(std::string uniformName, glm::vec2 val); // vec2
	void setUniform(const char* uniformName, glm::vec2 val); // vec2
	void setUniform(std::string uniformName, glm::vec3 val); // vec3
	void setUniform(const char* uniformName, glm::vec3 val); // vec3
	void setUniform(std::string uniformName, glm::vec4 val); // vec4
	void setUniform(const char* uniformName, glm::vec4 val); // vec4
	
	void setUniform(std::string uniformName, glm::mat2   val); // mat2
	void setUniform(const char* uniformName, glm::mat2   val); // mat2
	void setUniform(std::string uniformName, glm::mat2x3 val); // mat2x3
	void setUniform(const char* uniformName, glm::mat2x3 val); // mat2x3
	void setUniform(std::string uniformName, glm::mat2x4 val); // mat2x4
	void setUniform(const char* uniformName, glm::mat2x4 val); // mat2x4
	
	void setUniform(std::string uniformName, glm::mat3   val); // mat3
	void setUniform(const char* uniformName, glm::mat3   val); // mat3
	void setUniform(std::string uniformName, glm::mat3x2 val); // mat3x2
	void setUniform(const char* uniformName, glm::mat3x2 val); // mat3x2
	void setUniform(std::string uniformName, glm::mat3x4 val); // mat3x4
	void setUniform(const char* uniformName, glm::mat3x4 val); // mat3x4
	
	void setUniform(std::string uniformName, glm::mat4   val); // mat4
	void setUniform(const char* uniformName, glm::mat4   val); // mat4
	void setUniform(std::string uniformName, glm::mat4x2 val); // mat4x2
	void setUniform(const char* uniformName, glm::mat4x2 val); // mat4x2
	void setUniform(std::string uniformName, glm::mat4x3 val); // mat4x3
	void setUniform(const char* uniformName, glm::mat4x3 val); // mat4x3

private:
	//ShaderString parseShader(std::string path);
	void compileShader(unsigned int type, std::string& source);

public:
	unsigned int id;
private:
	std::string vPath;
	std::string fPath;
};

