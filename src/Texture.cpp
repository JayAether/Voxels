#include "Texture.h"

#include <glad/glad.h>
//#include <stb_image.h>
#include "stb_image.h"

#include <expected>
#include <iostream>

Texture::Texture()
	: m_width(0), m_height(0), m_colorChannels(0)
{
	glCreateTextures(GL_TEXTURE_2D, 1, &m_id);


}

Texture::~Texture()
{
	glDeleteTextures(1, &m_id);
}

TextureError Texture::loadTexture(std::string filepath)
{
	std::expected<unsigned char*, TextureErrorData> src = readImage(filepath, &m_width, &m_height, &m_colorChannels);
	if (!src)
	{
		std::cout << "ERROR - texture loading failed :: FILEPATH - " << src.error().filepath << " MESSAGE - " << src.error().msg << '\n';
		return src.error().err;
	}

	glTextureStorage2D(m_id, 1, GL_RGB8, m_width, m_height);
	glTextureSubImage2D(m_id, 0, 0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, src.value());

	glTextureParameteri(m_id, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(m_id, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTextureParameteri(m_id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTextureParameteri(m_id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glGenerateTextureMipmap(m_id);

	stbi_image_free(src.value());
	return TextureError::NA;
}

void Texture::bind(uint32_t unit)
{
	glBindTextureUnit(unit, m_id);
}

std::expected<unsigned char*, TextureErrorData> readImage(std::string filepath, int32_t* width, int32_t* height, int32_t* colorChannels)
{
	stbi_set_flip_vertically_on_load(1);

	unsigned char* source = stbi_load(filepath.c_str(), width, height, colorChannels, 0);
	if (!source)
		return std::unexpected(TextureErrorData(TextureError::couldntOpenFile, stbi_failure_reason(), filepath));

	return source;
}