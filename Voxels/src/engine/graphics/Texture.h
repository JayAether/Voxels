#pragma once

#include <string>

class Texture
{
public:
	Texture(const std::string& path, unsigned int dimension = 2);
	~Texture();

	void loadFile();

	void bind(int bindTexTarget);
	void unbind();

	void setFilter();
	void setWrap();


private:
	std::string m_path;
	unsigned int m_dimension;
	unsigned int m_textureObj;
};

