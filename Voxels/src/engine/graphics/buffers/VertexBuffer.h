#pragma once


class VertexBuffer
{
public:
	VertexBuffer();
	VertexBuffer(const void* data, unsigned int size);
	~VertexBuffer();

	void Bind();
	void Unbind();

	void setData(const void* data, unsigned int size);

private:
	unsigned int m_rendererID;
};


