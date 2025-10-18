#pragma once


#include <string>
#include <expected>

enum class TextureError
{
	NA = 0,
	couldntOpenFile,
};

struct TextureErrorData
{
	TextureError err;
	std::string msg;
	std::string filepath;

	TextureErrorData(TextureError err, std::string msg, std::string filepath)
		: err(err), msg(msg), filepath(filepath) { }
};


class Texture
{
public:
	Texture();
	~Texture();

	TextureError loadTexture(std::string filepath);

	void bind(uint32_t index);

private:
	uint32_t m_id;

	int32_t m_width, m_height, m_colorChannels;



};


// WARNING - this doesnt free up data, so be sure to call `stbi_image_free` after your done with the image
std::expected<unsigned char*, TextureErrorData> readImage(std::string filepath, int32_t* width, int32_t* height, int32_t* colorChannels);
