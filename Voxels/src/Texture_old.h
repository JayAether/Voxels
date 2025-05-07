#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../3rdparty/stb_image.h"

#include <string>

class Texture_old
{
	std::string m_Filename;
	GLenum m_TextureTarget;
	GLuint m_TextureObj;
public:
	Texture_old(GLenum textureTarget, const std::string& filename);

	bool load();

	void bind(GLenum textureUnit);

private:
};

