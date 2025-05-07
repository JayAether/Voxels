#include "Texture_old.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../3rdparty/stb_image.h"

#include <string>
#include <iostream>

// textureTargets option include: GL_TEXTURE_1D, GL_TEXTURE_2D, GL_TEXTURE_3D
Texture_old::Texture_old(GLenum textureTarget, const std::string& filename)
{
	m_TextureTarget = textureTarget;
	m_Filename = filename;
}

bool Texture_old::load()
{
	// load texture image
	stbi_set_flip_vertically_on_load(1);
	int width, height, nrChannels;
	unsigned char* image_data = stbi_load(m_Filename.c_str(), &width, &height, &nrChannels, 0);

	if (!image_data)
	{
		std::cout << "failed to filename: " << m_Filename << "\t: error message: " << stbi_failure_reason();
		exit(0);
	}

	//printf("width %d, height %d, nrChannels %d\n", width, height, nrChannels);


	// gen texture

	glGenTextures(1, &m_TextureObj);
	glBindTexture(m_TextureTarget, m_TextureObj);
	if (m_TextureTarget == GL_TEXTURE_2D)
	{
		glTexImage2D(m_TextureTarget, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
	}
	else
	{
		printf("support for texture target %x is non existant you bitch\n", m_TextureTarget);
		exit(1);
	}
	
	// basic params, probably should make this its own thing
	glTexParameterf(m_TextureTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameterf(m_TextureTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterf(m_TextureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(m_TextureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// clean
	glBindTexture(m_TextureTarget, 0); // free target from binding
	stbi_image_free(image_data); // clean std_image

	return true;
}

void Texture_old::bind(GLenum textureUnit)
{
	glActiveTexture(textureUnit);
	glBindTexture(m_TextureTarget, m_TextureObj);
}