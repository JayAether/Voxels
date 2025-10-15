#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <string>
#include <vector>


class ShaderProgram
{
    struct UniformLocation
    {
        UniformLocation(std::string, GLint location)
        {
            this->name = name;
            this->location = location;
        }

        std::string name;
        GLint location;
    };

    std::vector<UniformLocation> m_uniformLocations;


public:
    unsigned int ID;

    ShaderProgram(const char* vertexPath, const char* fragmentPath);
    
    ~ShaderProgram();

    void use();

    void setValue(std::string name, int value);
    void setValue(std::string name, float value);
    void setValue(std::string name, glm::mat4 value);
    void setValue(std::string name, glm::vec3 value);
    void setValue(std::string name, glm::ivec3 value);

private:
    
    bool getPreExistingUniformLoc(std::string name, GLint& ret);
};









//#include <glad/glad.h>
//#include <GLFW/glfw3.h>
//#include <glm/glm.hpp>
//#include <glm/gtc/type_ptr.hpp>
//
//#include <fstream>
//#include <sstream>
//#include <string>
//#include <iostream>
//
//class ShaderProgram
//{
//public:
//    unsigned int ID;
//
//
//	ShaderProgram(const char* vertexPath, const char* fragmentPath)
//	{
//		// load shaders
//        std::string vertexCode;
//        std::string fragmentCode;
//
//        std::ifstream vShaderFile;
//        std::ifstream fShaderFile;
//
//        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
//        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
//
//        try
//        {
//            vShaderFile.open(vertexPath);
//            fShaderFile.open(fragmentPath);
//
//            std::stringstream vShaderStream, fShaderStream;
//
//            vShaderStream << vShaderFile.rdbuf();
//            fShaderStream << fShaderFile.rdbuf();
//
//            vShaderFile.close();
//            fShaderFile.close();
//
//            vertexCode = vShaderStream.str();
//            fragmentCode = fShaderStream.str();
//        }
//        catch (std::ifstream::failure& e)
//        {
//            std::cout << "failed to read shader file: " << e.what() << std::endl;
//            std::cout << vertexCode;
//            std::cout << fragmentCode;
//        }
//
//        const char* vShaderCode = vertexCode.c_str();
//        const char* fShaderCode = fragmentCode.c_str();
//
//
//
//        int vertex, fragment;
//        int success;
//        char infoLog[1024];
//
//        vertex = glCreateShader(GL_VERTEX_SHADER);
//        glShaderSource(vertex, 1, &vShaderCode, NULL);
//        glCompileShader(vertex);
//        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
//        if (!success)
//        {
//            glGetShaderInfoLog(vertex, 1024, NULL, infoLog);
//            std::cout << "failed to load vertex shader: " << infoLog << std::endl;
//        }
//
//        fragment = glCreateShader(GL_FRAGMENT_SHADER);
//        glShaderSource(fragment, 1, &fShaderCode, NULL);
//        glCompileShader(fragment);
//        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
//        if (!success)
//        {
//            glGetShaderInfoLog(fragment, 1024, NULL, infoLog);
//            std::cout << "failed to load fragment shader: " << infoLog << std::endl;
//        }
//
//        ID = glCreateProgram();
//        glAttachShader(ID, vertex);
//        glAttachShader(ID, fragment);
//        glLinkProgram(ID);
//        glGetProgramiv(ID, GL_LINK_STATUS, &success);
//        if (!success)
//        {
//            glGetProgramInfoLog(ID, 1024, NULL, infoLog);
//            std::cout << "failed to load shader program: " << infoLog << std::endl;
//        }
//
//        glDeleteShader(vertex);
//        glDeleteShader(fragment);
//	}
//
//    // sets the program to be the current active program
//    void use()
//    {
//        glUseProgram(ID);
//    }
//
//    // set the uniform
//    // c++ type int
//    // glsl type int
//    void setValue(std::string name, int value)
//    {
//        glProgramUniform1i(ID, glGetUniformLocation(ID, name.c_str()), value);
//    }
//    // set uniform
//    // c++ type float
//    // glsl type float
//    void setValue(std::string name, float value)
//    {
//        glProgramUniform1f(ID, glGetUniformLocation(ID, name.c_str()), value);
//    }
//    // set uniform
//    // c++ type glm::mat4
//    // glsl type vec4
//    void setValue(std::string name, glm::mat4 value)
//    {
//        glProgramUniformMatrix4fv(ID, glGetUniformLocation(ID, name.c_str()), 1, NULL, glm::value_ptr(value));
//    }
////private:
//
//};
