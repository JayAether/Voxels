#include "Texture.h"

#include <glad/glad.h>

#include "../../../3rdparty/stb_image.h"

#include <system_error>

Texture::Texture(const std::string& path, unsigned int dimension)
	: m_path(path)
{
	glGenTextures(1, &m_textureObj);

	switch (dimension)
	{
	case 1:
		dimension = GL_TEXTURE_1D;
		break;
	case 2:
		dimension = GL_TEXTURE_2D;
		break;
	case 3:
		dimension = GL_TEXTURE_3D;
		break;
	default:
		std::string err_msg = "non valid texture dimension\nfile path: " + m_path + "\n";
		throw std::runtime_error(err_msg);
		break;
	}

	loadFile();
}

Texture::~Texture()
{

}

void Texture::loadFile()
{
	stbi_set_flip_vertically_on_load(1);
	
	int width, height, channels;
	unsigned char* data = stbi_load(m_path.c_str(), &width, &height, &channels, 0);
	if (!data)
	{
		std::string err_msg = "failed to load image\nfile path: " + m_path + "\nerror message: " + stbi_failure_reason() + "\n";
		throw std::runtime_error(err_msg);
	}

	glBindTexture(m_dimension, m_textureObj);
	if (m_dimension == GL_TEXTURE_2D);
	{
		glTexImage2D(m_dimension, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	}

	// defaults
	glTexParameterf(m_textureObj, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameterf(m_textureObj, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterf(m_textureObj, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(m_textureObj, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	stbi_image_free(data);
	glBindTexture(m_dimension, 0);
}	


void Texture::bind(int texTarget)
{
	glActiveTexture(GL_TEXTURE0 + texTarget);
	glBindTexture(m_dimension, m_textureObj);
}

void Texture::unbind()
{
	glBindTexture(m_dimension, 0);
}

void Texture::setFilter()
{

}

void Texture::setWrap()
{

}