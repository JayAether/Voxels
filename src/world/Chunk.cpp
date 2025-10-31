#include "Chunk.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>

#include "../3rdparty/FastNoiseLite.h"

#include <vector>
#include <algorithm>

typename uint32_t vertAttribPos	= 0;
typename uint32_t vertAttribNorm = 1;
typename uint32_t vertAttribTC	= 2;

typename uint32_t vertBufferBinding = 0;


Chunk::Chunk(chunkOffset chunkpos)
	: m_chunkOffset(chunkpos)
{
	glCreateVertexArrays(1, &m_VAO);
	glCreateBuffers(1, &m_vb);
	glCreateBuffers(1, &m_ib);


	m_active.resize(CHUNK_LEN);

	for (uint32_t i = 0; i < CHUNK_LEN; i++)
	{
		m_active.at(i).resize(CHUNK_LEN);
		for (uint32_t j = 0; j < CHUNK_LEN; j++)
		{
			m_active.at(i).at(j) = 0;
		}
	}
	

	generateMap();
}

Chunk::~Chunk()
{
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_vb);
	glDeleteBuffers(1, &m_ib);
}

void Chunk::render(ShaderProgram program)
{
	 //this exists so opengl doesnt freakout if chunk is empty
	if (m_vertData.size() == 0) return;
	
	program.use();

	program.setValue("chunkOffset", m_chunkOffset);
	program.setValue("chunkSize", CHUNK_LEN);
	glm::mat4 model = glm::mat4(1.0f);
	program.setValue("model", model);


	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, m_vertIndices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Chunk::generateMap()
{
	glm::vec3 chunkPos; 
	chunkPos = m_chunkOffset * (int32_t)CHUNK_LEN;

	FastNoiseLite noise;
	float chunkHeightMap[CHUNK_AREA];

	for (uint32_t x = 0; x < CHUNK_LEN; x++)
	{
		for (uint32_t z = 0; z < CHUNK_LEN; z++)
		{
			uint32_t index = getBlockIndex2D(x, z);

			chunkHeightMap[index] = noise.GetNoise(x + chunkPos.x, z + chunkPos.z)* 15.0f + 16.0f;
		}
	}



	for (int x = 0; x < CHUNK_LEN; x++)
	{

		for (int z = 0; z < CHUNK_LEN; z++)
		{
			int index = getBlockIndex2D(x, z);

			for (uint32_t y = 0; y < CHUNK_LEN; y++)
			{
				if (y < chunkHeightMap[index])
					m_active.at(x).at(y)[z] = 1;
			}
		}
	}

}

void Chunk::bake(std::vector<std::shared_ptr<Chunk>> neighbourChunks)
{
	for (uint32_t x = 0; x < CHUNK_LEN; x++)
	{
		for (uint32_t y = 0; y < CHUNK_LEN; y++)
		{
			for (uint32_t z = 0; z < CHUNK_LEN; z++)
			{

				if (m_active.at(x).at(y)[z] == 0) continue;

				if (x		> 0			&& m_active.at(x - 1).at(y)[z] == 0) loadFaceNegativeX(x, y, z);  // neg x
				if (x + 1	< CHUNK_LEN	&& m_active.at(x + 1).at(y)[z] == 0) loadFacePositiveX(x, y, z);  // pos x
				if (y		> 0			&& m_active.at(x).at(y - 1)[z] == 0) loadFaceNegativeY(x, y, z);  // neg y
				if (y + 1	< CHUNK_LEN	&& m_active.at(x).at(y + 1)[z] == 0) loadFacePositiveY(x, y, z);  // pos y
				if (z		> 0			&& m_active.at(x).at(y)[z - 1] == 0) loadFaceNegativeZ(x, y, z);  // neg z
				if (z + 1	< CHUNK_LEN	&& m_active.at(x).at(y)[z + 1] == 0) loadFacePositiveZ(x, y, z);  // pos z
			}
		}
	}


	for (uint32_t k = 0; k < neighbourChunks.size(); k++)
	{
		chunkOffset otherChunk = neighbourChunks.at(k)->getChunkOffset() - m_chunkOffset;

		for (uint32_t i = 0; i < CHUNK_LEN; i++)
		{
			for (uint32_t j = 0; j < CHUNK_LEN; j++)
			{
				// triggers if THIS block is active, and the NEIGHBOUR block is innactive
				if (otherChunk == glm::ivec3(-1,  0,  0) && m_active.at(0).at(i)[j] == 1				&& !neighbourChunks.at(k)->blockStateActive(glm::ivec3(CHUNK_LEN - 1, i, j))) loadFaceNegativeX(0, i, j);
				if (otherChunk == glm::ivec3( 1,  0,  0) && m_active.at(CHUNK_LEN - 1).at(i)[j] == 1	&& !neighbourChunks.at(k)->blockStateActive(glm::ivec3(0, i, j))) loadFacePositiveX(CHUNK_LEN - 1, i, j);
				if (otherChunk == glm::ivec3( 0, -1,  0) && m_active.at(i).at(0)[j] == 1				&& !neighbourChunks.at(k)->blockStateActive(glm::ivec3(0, CHUNK_LEN - 1, j))) loadFaceNegativeY(i, 0, j);
				if (otherChunk == glm::ivec3( 0,  1,  0) && m_active.at(i).at(CHUNK_LEN - 1)[j] == 1	&& !neighbourChunks.at(k)->blockStateActive(glm::ivec3(i, 0, j))) loadFacePositiveY(i, CHUNK_LEN - 1, j);
				if (otherChunk == glm::ivec3( 0,  0, -1) && m_active.at(i).at(j)[0] == 1				&& !neighbourChunks.at(k)->blockStateActive(glm::ivec3(i, j, CHUNK_LEN - 1))) loadFaceNegativeZ(i, j, 0);
				if (otherChunk == glm::ivec3( 0,  0,  1) && m_active.at(i).at(j)[CHUNK_LEN - 1] == 1	&& !neighbourChunks.at(k)->blockStateActive(glm::ivec3(i, j, 0))) loadFacePositiveZ(i, j, CHUNK_LEN - 1);
			}
		}
	}
}

bool Chunk::blockStateActive(glm::ivec3 relativeBlockPos)
{
	if (relativeBlockPos.x < 0 || relativeBlockPos.x > CHUNK_LEN - 1 ||
		relativeBlockPos.y < 0 || relativeBlockPos.x > CHUNK_LEN - 1 ||
		relativeBlockPos.z < 0 || relativeBlockPos.z > CHUNK_LEN - 1)
		return false;

	return m_active.at(relativeBlockPos.x).at(relativeBlockPos.y)[relativeBlockPos.z];
}

void Chunk::clearData()
{
	m_vertData.clear();
	m_vertIndices.clear();
}

void Chunk::populateBuffers()
{
	// this exists so opengl doesnt freakout if chunk is empty
	if (m_vertData.size() == 0) return;
	
	constexpr const size_t vertStride = sizeof(m_vertData[0]);

	constexpr const size_t posOffset = offsetof(vertex, pos);
	constexpr const size_t normOffset = offsetof(vertex, norm);
	constexpr const size_t tcOffset = offsetof(vertex, tc);

	// values per data
	constexpr const uint32_t posSize	= 3;
	constexpr const uint32_t normSize	= 3;
	constexpr const uint32_t tcSize		= 2;

	const size_t vertSize	 = sizeof(m_vertData[0])	* m_vertData.size();
	const size_t indicesSize = sizeof(m_vertIndices[0])	* m_vertIndices.size();

	
	glNamedBufferStorage(m_vb, vertSize,	m_vertData.data(), GL_DYNAMIC_STORAGE_BIT);
	glNamedBufferStorage(m_ib, indicesSize, m_vertIndices.data(), GL_DYNAMIC_STORAGE_BIT);

	glVertexArrayVertexBuffer(m_VAO, vertBufferBinding, m_vb, 0, vertStride);
	glVertexArrayElementBuffer(m_VAO, m_ib);

	glVertexArrayAttribBinding(m_VAO, vertAttribPos,	vertBufferBinding);
	glVertexArrayAttribBinding(m_VAO, vertAttribNorm,	vertBufferBinding);
	glVertexArrayAttribBinding(m_VAO, vertAttribTC,		vertBufferBinding);

	glEnableVertexArrayAttrib(m_VAO, vertAttribPos);
	glEnableVertexArrayAttrib(m_VAO, vertAttribNorm);
	glEnableVertexArrayAttrib(m_VAO, vertAttribTC);

	glVertexArrayAttribFormat(m_VAO, vertAttribPos,	 posSize,	GL_FLOAT, GL_FALSE, posOffset);
	glVertexArrayAttribFormat(m_VAO, vertAttribNorm, normSize,	GL_FLOAT, GL_FALSE, normOffset);
	glVertexArrayAttribFormat(m_VAO, vertAttribTC,	 tcSize,	GL_FLOAT, GL_FALSE, tcOffset);
}

chunkOffset Chunk::getChunkOffset() const { return m_chunkOffset; }

int Chunk::getBlockIndex3D(int x, int y, int z) const { return (x) + (y * CHUNK_LEN) + (z * CHUNK_AREA); }
int Chunk::getBlockIndex2D(int x, int y) const { return x + y * CHUNK_LEN; }

blockKey Chunk::getBlockKey(uint32_t x, uint32_t y, uint32_t z)
{
	static_assert(sizeof(CHUNK_LEN) == sizeof(uint8_t)); // chunk len must be a uint8, due to the key expecting it to be that size

	x <<= 16;
	y <<= 8;

	blockKey key;
	key |= z;
	key |= y;
	key |= x;

	return key;
}

void Chunk::loadFaceNegativeX(int x, int y, int z)
{
	glm::vec3 bottomRight	(x * 2 - BLOCK_RENDER_SIZE, y * 2 - BLOCK_RENDER_SIZE, z * 2 + BLOCK_RENDER_SIZE);
	glm::vec3 topRight		(x * 2 - BLOCK_RENDER_SIZE, y * 2 + BLOCK_RENDER_SIZE, z * 2 + BLOCK_RENDER_SIZE);
	glm::vec3 bottomLeft	(x * 2 - BLOCK_RENDER_SIZE, y * 2 - BLOCK_RENDER_SIZE, z * 2 - BLOCK_RENDER_SIZE);
	glm::vec3 topLeft		(x * 2 - BLOCK_RENDER_SIZE, y * 2 + BLOCK_RENDER_SIZE, z * 2 - BLOCK_RENDER_SIZE);

	uint32_t offset = m_vertData.size();
	m_vertIndices.emplace_back(offset + 0);
	m_vertIndices.emplace_back(offset + 1);
	m_vertIndices.emplace_back(offset + 2);
	m_vertIndices.emplace_back(offset + 2);
	m_vertIndices.emplace_back(offset + 3);
	m_vertIndices.emplace_back(offset + 0);


	m_vertData.push_back(vertex(topRight,		glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)));
	m_vertData.push_back(vertex(bottomRight,	glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)));
	m_vertData.push_back(vertex(bottomLeft,		glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)));
	m_vertData.push_back(vertex(topLeft,		glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)));
}

void Chunk::loadFacePositiveX(int x, int y, int z)
{
	glm::vec3 bottomLeft	(x * 2 + BLOCK_RENDER_SIZE, y * 2 - BLOCK_RENDER_SIZE, z * 2 + BLOCK_RENDER_SIZE);
	glm::vec3 topLeft		(x * 2 + BLOCK_RENDER_SIZE, y * 2 + BLOCK_RENDER_SIZE, z * 2 + BLOCK_RENDER_SIZE);
	glm::vec3 bottomRight	(x * 2 + BLOCK_RENDER_SIZE, y * 2 - BLOCK_RENDER_SIZE, z * 2 - BLOCK_RENDER_SIZE);
	glm::vec3 topRight		(x * 2 + BLOCK_RENDER_SIZE, y * 2 + BLOCK_RENDER_SIZE, z * 2 - BLOCK_RENDER_SIZE);

	uint32_t offset = m_vertData.size();
	m_vertIndices.emplace_back(offset + 0);
	m_vertIndices.emplace_back(offset + 1);
	m_vertIndices.emplace_back(offset + 2);
	m_vertIndices.emplace_back(offset + 2);
	m_vertIndices.emplace_back(offset + 3);
	m_vertIndices.emplace_back(offset + 0);

	m_vertData.push_back(vertex(topRight,		glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)));
	m_vertData.push_back(vertex(bottomRight,	glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)));
	m_vertData.push_back(vertex(bottomLeft,		glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)));
	m_vertData.push_back(vertex(topLeft,		glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)));
}

void Chunk::loadFaceNegativeY(int x, int y, int z)
{
	glm::vec3 topLeft		(x * 2 - BLOCK_RENDER_SIZE, y * 2 - BLOCK_RENDER_SIZE, z * 2 + BLOCK_RENDER_SIZE);
	glm::vec3 topRight		(x * 2 + BLOCK_RENDER_SIZE, y * 2 - BLOCK_RENDER_SIZE, z * 2 + BLOCK_RENDER_SIZE);
	glm::vec3 bottomRight	(x * 2 + BLOCK_RENDER_SIZE, y * 2 - BLOCK_RENDER_SIZE, z * 2 - BLOCK_RENDER_SIZE);
	glm::vec3 bottomLeft	(x * 2 - BLOCK_RENDER_SIZE, y * 2 - BLOCK_RENDER_SIZE, z * 2 - BLOCK_RENDER_SIZE);

	uint32_t offset = m_vertData.size();
	m_vertIndices.emplace_back(offset + 0);
	m_vertIndices.emplace_back(offset + 1);
	m_vertIndices.emplace_back(offset + 2);
	m_vertIndices.emplace_back(offset + 2);
	m_vertIndices.emplace_back(offset + 3);
	m_vertIndices.emplace_back(offset + 0);

	m_vertData.push_back(vertex(topRight,		glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f)));
	m_vertData.push_back(vertex(bottomRight,	glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f)));
	m_vertData.push_back(vertex(bottomLeft,		glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f)));
	m_vertData.push_back(vertex(topLeft,		glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f)));
}

void Chunk::loadFacePositiveY(int x, int y, int z)
{
	glm::vec3 bottomRight	(x * 2 + BLOCK_RENDER_SIZE, y * 2 + BLOCK_RENDER_SIZE, z * 2 + BLOCK_RENDER_SIZE);
	glm::vec3 bottomLeft	(x * 2 - BLOCK_RENDER_SIZE, y * 2 + BLOCK_RENDER_SIZE, z * 2 + BLOCK_RENDER_SIZE);
	glm::vec3 topLeft		(x * 2 - BLOCK_RENDER_SIZE, y * 2 + BLOCK_RENDER_SIZE, z * 2 - BLOCK_RENDER_SIZE);
	glm::vec3 topRight		(x * 2 + BLOCK_RENDER_SIZE, y * 2 + BLOCK_RENDER_SIZE, z * 2 - BLOCK_RENDER_SIZE);

	uint32_t offset = m_vertData.size();
	m_vertIndices.emplace_back(offset + 0);
	m_vertIndices.emplace_back(offset + 1);
	m_vertIndices.emplace_back(offset + 2);
	m_vertIndices.emplace_back(offset + 2);
	m_vertIndices.emplace_back(offset + 3);
	m_vertIndices.emplace_back(offset + 0);

	m_vertData.push_back(vertex(topRight,		glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f)));
	m_vertData.push_back(vertex(bottomRight,	glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f)));
	m_vertData.push_back(vertex(bottomLeft,		glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f)));
	m_vertData.push_back(vertex(topLeft,		glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f)));
}

void Chunk::loadFaceNegativeZ(int x, int y, int z)
{
	glm::vec3 bottomLeft	(x * 2 + BLOCK_RENDER_SIZE, y * 2 - BLOCK_RENDER_SIZE, z * 2 - BLOCK_RENDER_SIZE);
	glm::vec3 bottomRight	(x * 2 - BLOCK_RENDER_SIZE, y * 2 - BLOCK_RENDER_SIZE, z * 2 - BLOCK_RENDER_SIZE);
	glm::vec3 topRight		(x * 2 - BLOCK_RENDER_SIZE, y * 2 + BLOCK_RENDER_SIZE, z * 2 - BLOCK_RENDER_SIZE);
	glm::vec3 topLeft		(x * 2 + BLOCK_RENDER_SIZE, y * 2 + BLOCK_RENDER_SIZE, z * 2 - BLOCK_RENDER_SIZE);

	uint32_t offset = m_vertData.size();
	m_vertIndices.emplace_back(offset + 0);
	m_vertIndices.emplace_back(offset + 1);
	m_vertIndices.emplace_back(offset + 2);
	m_vertIndices.emplace_back(offset + 2);
	m_vertIndices.emplace_back(offset + 3);
	m_vertIndices.emplace_back(offset + 0);

	m_vertData.push_back(vertex(topRight,		glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f)));
	m_vertData.push_back(vertex(bottomRight,	glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f)));
	m_vertData.push_back(vertex(bottomLeft,		glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f)));
	m_vertData.push_back(vertex(topLeft,		glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f)));
}

void Chunk::loadFacePositiveZ(int x, int y, int z)
{
	glm::vec3 bottomLeft	(x * 2 - BLOCK_RENDER_SIZE, y * 2 - BLOCK_RENDER_SIZE, z * 2 + BLOCK_RENDER_SIZE);
	glm::vec3 bottomRight	(x * 2 + BLOCK_RENDER_SIZE, y * 2 - BLOCK_RENDER_SIZE, z * 2 + BLOCK_RENDER_SIZE);
	glm::vec3 topRight		(x * 2 + BLOCK_RENDER_SIZE, y * 2 + BLOCK_RENDER_SIZE, z * 2 + BLOCK_RENDER_SIZE);
	glm::vec3 topLeft		(x * 2 - BLOCK_RENDER_SIZE, y * 2 + BLOCK_RENDER_SIZE, z * 2 + BLOCK_RENDER_SIZE);

	uint32_t offset = m_vertData.size();
	m_vertIndices.emplace_back(offset + 0);
	m_vertIndices.emplace_back(offset + 1);
	m_vertIndices.emplace_back(offset + 2);
	m_vertIndices.emplace_back(offset + 2);
	m_vertIndices.emplace_back(offset + 3);
	m_vertIndices.emplace_back(offset + 0);

	m_vertData.push_back(vertex(topRight,		glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f)));
	m_vertData.push_back(vertex(bottomRight,	glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f)));
	m_vertData.push_back(vertex(bottomLeft,		glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f)));
	m_vertData.push_back(vertex(topLeft,		glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f)));
}
