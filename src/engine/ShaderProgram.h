#pragma once


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

    void cleanup();

    void use();

    void setValue(std::string name, int value);
    void setValue(std::string name, float value);
    void setValue(std::string name, glm::mat4 value);
    void setValue(std::string name, glm::vec3 value);
    void setValue(std::string name, glm::ivec3 value);

private:
    
    bool getPreExistingUniformLoc(std::string name, GLint& ret);
};






