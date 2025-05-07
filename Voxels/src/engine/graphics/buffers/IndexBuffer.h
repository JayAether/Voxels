#pragma once


class IndexBuffer
{
public:
	IndexBuffer(const unsigned int* data, unsigned int count);
	~IndexBuffer();

	void Bind();
	void Unbind();

	void setData(const unsigned int* data, unsigned int count);


private:
	unsigned int m_rendererID;
	unsigned int m_count;

};


