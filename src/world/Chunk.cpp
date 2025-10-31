#include "Chunk.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>

#include "../3rdparty/FastNoiseLite.h"

#include <vector>
#include <algorithm>

typename int32_t vertAttribPos	= 0;
typename int32_t vertAttribNorm = 1;
typename int32_t vertAttribTC	= 2;

typename int32_t bindingIndexPos	= vertAttribPos;
typename int32_t bindingIndexNorm	= vertAttribNorm;
typename int32_t bindingIndexTC		= vertAttribTC;


Chunk::Chunk(chunkOffset chunkpos)
	: m_chunkOffset(chunkpos)
{
	glCreateVertexArrays(1, &m_VAO);
	glCreateBuffers(NUM_BUFFERS, m_vb);
	glCreateBuffers(1, &vb);
	glGenBuffers(1, &m_ib);


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
	glDeleteBuffers(NUM_BUFFERS, m_vb);
	glDeleteBuffers(1, &vb);
	glDeleteBuffers(1, &m_ib);

}

void Chunk::render(ShaderProgram program)
{

	 //this exists so opengl doesnt freakout if chunk is empty
	if (m_vertDataAOS.size() == 0) return;
	

	program.use();

	program.setValue("chunkOffset", m_chunkOffset);
	program.setValue("chunkSize", CHUNK_LEN);
	glm::mat4 model = glm::mat4(1.0f);
	program.setValue("model", model);


	glBindVertexArray(m_VAO);
	//glDrawArrays(GL_TRIANGLES, 0, m_vertData.size());
	glDrawElements(GL_TRIANGLES, m_vertIndices.size()*6, GL_UNSIGNED_INT, 0);
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
				if (otherChunk == glm::ivec3(-1,  0,  0) && m_active.at(0).at(i)[j] == 1				&& !neighbourChunks.at(k)->blockStateActive(glm::ivec3(CHUNK_LEN - 1, i, j))) loadFaceNegativeX(0, i, j);
				if (otherChunk == glm::ivec3( 1,  0,  0) && m_active.at(CHUNK_LEN - 1).at(i)[j] == 1	&& !neighbourChunks.at(k)->blockStateActive(glm::ivec3(0, i, j))) loadFacePositiveX(CHUNK_LEN - 1, i, j);
				if (otherChunk == glm::ivec3( 0, -1,  0) && m_active.at(i).at(0)[j] == 1				&& !neighbourChunks.at(k)->blockStateActive(glm::ivec3(0, CHUNK_LEN - 1, j))) loadFaceNegativeY(i, 0, j);
				if (otherChunk == glm::ivec3( 0,  1,  0) && m_active.at(i).at(CHUNK_LEN - 1)[j] == 1	&& !neighbourChunks.at(k)->blockStateActive(glm::ivec3(i, 0, j))) loadFacePositiveY(i, CHUNK_LEN - 1, j);
				if (otherChunk == glm::ivec3( 0,  0, -1) && m_active.at(i).at(j)[0] == 1				&& !neighbourChunks.at(k)->blockStateActive(glm::ivec3(i, j, CHUNK_LEN - 1))) loadFaceNegativeZ(i, j, 0);
				if (otherChunk == glm::ivec3( 0,  0,  1) && m_active.at(i).at(j)[CHUNK_LEN - 1] == 1	&& !neighbourChunks.at(k)->blockStateActive(glm::ivec3(i, j, 0))) loadFacePositiveZ(i, j, CHUNK_LEN - 1);
			}
		}
	}

	generateGLBufferData();
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
	m_vertDataAOS.clear();
	m_vertPos.clear();
	m_vertNorm.clear();
	m_vertTC.clear();
	m_vertIndices.clear();
}

void Chunk::populateBuffers()
{
	// this exists so opengl doesnt freakout if chunk is empty
	if (m_vertDataAOS.size() == 0) return;


	/*glNamedBufferData(m_vb[POS],		sizeof(m_vertPos[0])		* m_vertPos.size(),		m_vertPos.data(),		GL_STATIC_DRAW);
	glNamedBufferData(m_vb[NORM],		sizeof(m_vertNorm[0])		* m_vertNorm.size(),	m_vertNorm.data(),		GL_STATIC_DRAW);
	glNamedBufferData(m_vb[TEX_COORD],	sizeof(m_vertTC[0])			* m_vertTC.size(),		m_vertTC.data(),		GL_STATIC_DRAW);*/

	glBindVertexArray(m_VAO);

	size_t posSize	= sizeof(m_vertPos[0]) * m_vertPos.size();
	size_t normSize = sizeof(m_vertNorm[0]) * m_vertNorm.size();
	size_t tcSize = sizeof(m_vertTC[0]) * m_vertTC.size();
	size_t indicesSize= sizeof(m_vertIndices[0]) * m_vertIndices.size();


	//glBindBuffer(GL_ARRAY_BUFFER, m_vb[POS]);
	//glBufferData(GL_ARRAY_BUFFER, posSize, m_vertPos.data(), GL_STATIC_DRAW);
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	////glBindBuffer(GL_ARRAY_BUFFER, 0);

	//glBindBuffer(GL_ARRAY_BUFFER, m_vb[NORM]);
	//glBufferData(GL_ARRAY_BUFFER, normSize, m_vertNorm.data(), GL_STATIC_DRAW);
	//glEnableVertexAttribArray(1);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	////glBindBuffer(GL_ARRAY_BUFFER, 0);

	//glBindBuffer(GL_ARRAY_BUFFER, m_vb[TEX_COORD]);
	//glBufferData(GL_ARRAY_BUFFER, tcSize, m_vertTC.data(), GL_STATIC_DRAW);
	//glEnableVertexAttribArray(2);
	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
	////glBindBuffer(GL_ARRAY_BUFFER, 0);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ib);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, m_vertIndices.data(), GL_STATIC_DRAW);

	//glBindBuffer(GL_ARRAY_BUFFER, vb);
	//glBufferData(GL_ARRAY_BUFFER, posSize + normSize + tcSize, nullptr, GL_STATIC_DRAW);

	//glBufferSubData(GL_ARRAY_BUFFER, 0,					 posSize,	m_vertPos.data());
	//glBufferSubData(GL_ARRAY_BUFFER, posSize,			 normSize,	m_vertNorm.data());
	//glBufferSubData(GL_ARRAY_BUFFER, posSize + normSize, tcSize,	m_vertTC.data());

	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)posSize);
	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)(posSize + normSize));

	//glEnableVertexAttribArray(0);
	//glEnableVertexAttribArray(1);
	//glEnableVertexAttribArray(2);


	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ib);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, m_vertIndices.data(), GL_STATIC_DRAW);



	//glVertexAttribPointer()
	//glBindBuffer(GL_ARRAY_BUFFER, 0);


	//glNamedBufferData(m_ib,	sizeof(m_vertIndices[0])	* m_vertIndices.size(), m_vertIndices.data(),	GL_STATIC_DRAW);

	//glVertexArrayAttribFormat(m_VAO, vertAttribPos,	3,	GL_FLOAT, GL_FALSE, 0);
	//glVertexArrayAttribFormat(m_VAO, vertAttribNorm, 3, GL_FLOAT, GL_FALSE, 0);
	//glVertexArrayAttribFormat(m_VAO, vertAttribTC, 2,	GL_FLOAT, GL_FALSE, 0);

	//glVertexArrayAttribBinding(m_VAO, vertAttribPos,	bindingIndexPos);
	//glVertexArrayAttribBinding(m_VAO, vertAttribNorm,	bindingIndexNorm);
	//glVertexArrayAttribBinding(m_VAO, vertAttribTC,		bindingIndexTC);

	//glVertexArrayVertexBuffer(m_VAO, bindingIndexPos,	m_vb[POS],		 (GLintptr)m_vertPos.data(),	sizeof(m_vertPos[0]));
	//glVertexArrayVertexBuffer(m_VAO, bindingIndexNorm,	m_vb[NORM],		 (GLintptr)m_vertNorm.data(),	sizeof(m_vertNorm[0]));
	//glVertexArrayVertexBuffer(m_VAO, bindingIndexTC,	m_vb[TEX_COORD], (GLintptr)m_vertTC.data(),		sizeof(m_vertTC[0]));
	//glVertexArrayElementBuffer(m_VAO, m_ib);

	//glEnableVertexArrayAttrib(m_VAO, vertAttribPos);
	//glEnableVertexArrayAttrib(m_VAO, vertAttribNorm);
	//glEnableVertexArrayAttrib(m_VAO, vertAttribTC);

	//glBindVertexBuffer(bindingIndexPos, m_vb[POS], 0, 0);


	//glBindVertexArray(m_VAO);
	//glVertexAttribPointer(vertAttribPos, 3, GL_FLOAT, GL_FALSE, 0, &m_vertPos[0]);
	//glVertexAttribPointer(vertAttribNorm, 3, GL_FLOAT, GL_FALSE, 0, &m_vertNorm[0]);
	//glVertexAttribPointer(vertAttribTC, 2, GL_FLOAT, GL_FALSE, 0, &m_vertTC[0]);
	//glBindVertexArray(0);



	glBindBuffer(GL_ARRAY_BUFFER, vb);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertDataAOS[0]) * m_vertDataAOS.size(), m_vertDataAOS.data(), GL_STATIC_DRAW);


	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(m_vertDataAOS[0]), (void*)0);
	glEnableVertexAttribArray(0);


	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(m_vertDataAOS[0]), (void*)(sizeof(glm::vec3)));
	glEnableVertexAttribArray(1);


	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(m_vertDataAOS[0]), (void*)(2*sizeof(glm::vec3)));
	glEnableVertexAttribArray(2);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ib);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize, m_vertIndices.data(), GL_STATIC_DRAW);


	glBindVertexArray(0);




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

void Chunk::generateGLBufferData()
{
	/*//for (uint32_t i = 0; i < m_vertData.size(); i++)
	//{
	//	//glm::vec3 pos = m_vertData.at(i).pos;
	//	//glm::vec3 norm = m_vertData.at(i).norm;
	//	//glm::vec2 texCoord = m_vertData.at(i).texCoord;

	//	//auto posLoc			= std::find(m_vertPos.begin(), m_vertPos.end(), pos);
	//	//auto normLoc		= std::find(m_vertNorm.begin(), m_vertNorm.end(), norm);
	//	//auto texCoordLoc	= std::find(m_vertTC.begin(), m_vertTC.end(), texCoord);

	//	//
	//	////int x = 0;
	//	//if (posLoc != m_vertPos.end())
	//	//	//m_vertIndices.emplace_back(std::distance(m_vertPos.begin(), posLoc));
	//	//	;
	//	//else 
	//	//{
	//	//	m_vertPos.emplace_back(pos);
	//	//	//m_vertIndices.emplace_back(i);
	//	//}

	//	//if (normLoc != m_vertNorm.end())
	//	//	//m_vertIndices.emplace_back(std::distance(m_vertNorm.begin(), normLoc));
	//	//	;
	//	//else
	//	//{
	//	//	m_vertNorm.emplace_back(norm);
	//	//	//m_vertIndices.emplace_back(i);
	//	//}

	//	//if (texCoordLoc != m_vertTC.end())
	//	//	//m_vertIndices.emplace_back(std::distance(m_vertTC.begin(), texCoordLoc));
	//	//	;
	//	//else
	//	//{
	//	//	m_vertTC.emplace_back(texCoord);
	//	//	//m_vertIndices.emplace_back(i);
	//	//}


	//		glm::vec3 pos = m_vertData.at(i).pos;
	//		bool posFound = false;
	//		for (uint32_t j = 0; j < m_vertPos.size(); j++)
	//		{
	//			if (m_vertPos.at(j) == pos)
	//			{
	//				posFound = true;
	//				break;
	//			}

	//			if (!posFound)
	//				m_vertPos.push_back(pos);



	//			glm::vec3 norm = m_vertData.at(i).norm;
	//			bool normFound = false;
	//			for (uint32_t j = 0; j < m_vertNorm.size(); j++)
	//			{
	//				if (m_vertNorm.at(j) == norm)
	//				{
	//					normFound = true;
	//					break;
	//				}
	//			}
	//			if (!normFound)
	//				m_vertNorm.push_back(norm);

	//			glm::vec2 texCoord = m_vertData.at(i).texCoord;
	//			bool tcFound = false;
	//			for (uint32_t j = 0; j < m_vertTC.size(); j++)
	//			{
	//				if (m_vertTC.at(j) == texCoord)
	//				{
	//					tcFound = true;
	//					break;
	//				}
	//			}
	//			if (!tcFound)
	//				m_vertTC.push_back(texCoord);


	//		}
	//	}


	//	//uint64_t largestArraySize = std::max<uint64_t>({ m_vertPos.size(), m_vertNorm.size(), m_vertTC.size() });

	//	//for (uint64_t i = 0; i < largestArraySize; i++)
	//	//{
	//	//	uint32_t posIndex;
	//	//	uint32_t normIndex;
	//	//	uint32_t tcIndex;

	//	//	for (uint32_t j = 0; j < m_vertPos.size(); j++)
	//	//	{
	//	//		if (m_vertPos.at(j) )
	//	//	}
	//	//}

	//	for (uint32_t i = 0; i < m_vertData.size(); i++)
	//	{
	//		uint32_t posIndex = 0;
	//		for (uint32_t j = 0; j < m_vertPos.size(); j++)
	//		{
	//			if (m_vertPos.at(j) == m_vertData.at(i).pos)
	//			{
	//				posIndex = j;
	//				break;
	//			}
	//		}
	//		m_vertIndices.push_back(posIndex);
	//	}


	//	for (uint32_t i = 0; i < m_vertData.size(); i++)
	//	{
	//		uint32_t normIndex = 0;
	//		for (uint32_t j = 0; j < m_vertNorm.size(); j++)
	//		{
	//			if (m_vertNorm.at(j) == m_vertData.at(i).norm)
	//			{
	//				normIndex = j;
	//				break;
	//			}
	//		}
	//		m_vertIndices.push_back(normIndex);
	//	}


	//	for (uint32_t i = 0; i < m_vertData.size(); i++)
	//	{
	//		uint32_t tcIndex = 0;
	//		for (uint32_t j = 0; j < m_vertTC.size(); j++)
	//		{
	//			if (m_vertTC.at(j) == m_vertData.at(i).texCoord)
	//			{
	//				tcIndex = j;
	//				break;
	//			}
	//		}


	//		m_vertIndices.push_back(tcIndex);
	//	}


	//	//auto normLoc = std::find(m_vertNorm.begin(), m_vertNorm.end(), m_vertData.at(i).norm);
	//	//auto texCoordLoc = std::find(m_vertTC.begin(), m_vertTC.end(), m_vertData.at(i).texCoord);

	//	//m_vertIndices.emplace_back(std::distance(m_vertPos.begin(), posLoc));
	//	//m_vertIndices.emplace_back(std::distance(m_vertNorm.begin(), normLoc));
	//	//m_vertIndices.emplace_back(std::distance(m_vertTC.begin(), texCoordLoc));

	////}*/








	//for (uint32_t i = 0; i < m_vertData.size(); i++)
	//{
	//	glm::vec3 pos = m_vertData.at(i).pos;
	//	bool posFound = false;
	//	for (uint32_t j = 0; j < m_vertPos.size(); j++)
	//	{
	//		if (m_vertPos.at(j) == pos)
	//		{
	//			posFound = true;
	//			break;
	//		}
	//	}
	//	if (!posFound)
	//		m_vertPos.push_back(pos);
	//	


	//	glm::vec3 norm = m_vertData.at(i).norm;
	//	bool normFound = false;
	//	for (uint32_t j = 0; j < m_vertNorm.size(); j++)
	//	{
	//		if (m_vertNorm.at(j) == norm)
	//		{
	//			normFound = true;
	//			break;
	//		}
	//	}
	//	if (!normFound)
	//		m_vertNorm.push_back(norm);


	//	glm::vec2 texCoord = m_vertData.at(i).texCoord;
	//	bool tcFound = false;
	//	for (uint32_t j = 0; j < m_vertTC.size(); j++)
	//	{
	//		if (m_vertTC.at(j) == texCoord)
	//		{
	//			tcFound = true;
	//			break;
	//		}
	//	}
	//	if (!tcFound)
	//		m_vertTC.push_back(texCoord);
	//}



	//for (uint32_t i = 0; i < m_vertData.size(); i++)
	//{
	//	uint32_t posIndex = i;
	//	bool found = false;
	//	for (uint32_t j = 0; j < m_vertPos.size(); j++)
	//	{
	//		posIndex = j;
	//		if (m_vertData.at(i).pos == m_vertPos.at(j))
	//		{
	//			
	//			found = true;
	//			break;
	//		}
	//	}
	//	if (found)
	//	{
	//		m_vertIndices.push_back(posIndex);
	//	}
	//}


	//for (uint32_t i = 0; i < m_vertData.size(); i++)
	//{
	//	uint32_t normIndex = 0;
	//	bool found = false;
	//	for (uint32_t j = 0; j < m_vertNorm.size(); j++)
	//	{
	//		normIndex = j;
	//		if (m_vertNorm.at(j) == m_vertData.at(i).norm)
	//		{
	//			found = true;
	//			break;
	//		}
	//	}
	//	if (found)
	//	{
	//		m_vertIndices.push_back(normIndex);
	//	}
	//}


	//for (uint32_t i = 0; i < m_vertData.size(); i++)
	//{
	//	uint32_t tcIndex = 0;
	//	for (uint32_t j = 0; j < m_vertTC.size(); j++)
	//	{
	//		if (m_vertTC.at(j) == m_vertData.at(i).texCoord)
	//		{
	//			tcIndex = j;
	//			break;
	//		}
	//	}


	//	m_vertIndices.push_back(tcIndex);
	//	m_vertIndices.push_back(tcIndex);
	//}




}

void Chunk::loadFaceNegativeX(int x, int y, int z)
{
	glm::vec3 bottomRight	(x * 2 - BLOCK_RENDER_SIZE, y * 2 - BLOCK_RENDER_SIZE, z * 2 + BLOCK_RENDER_SIZE);
	glm::vec3 topRight		(x * 2 - BLOCK_RENDER_SIZE, y * 2 + BLOCK_RENDER_SIZE, z * 2 + BLOCK_RENDER_SIZE);
	glm::vec3 bottomLeft	(x * 2 - BLOCK_RENDER_SIZE, y * 2 - BLOCK_RENDER_SIZE, z * 2 - BLOCK_RENDER_SIZE);
	glm::vec3 topLeft		(x * 2 - BLOCK_RENDER_SIZE, y * 2 + BLOCK_RENDER_SIZE, z * 2 - BLOCK_RENDER_SIZE);

	uint32_t offset = m_vertDataAOS.size();
	m_vertIndices.emplace_back(offset + 0);
	m_vertIndices.emplace_back(offset + 1);
	m_vertIndices.emplace_back(offset + 2);
	m_vertIndices.emplace_back(offset + 2);
	m_vertIndices.emplace_back(offset + 3);
	m_vertIndices.emplace_back(offset + 0);


	m_vertDataAOS.push_back(Vertex(topRight, glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)));
	m_vertDataAOS.push_back(Vertex(bottomRight, glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)));
	m_vertDataAOS.push_back(Vertex(bottomLeft,	glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)));
	m_vertDataAOS.push_back(Vertex(topLeft,		glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)));
	//m_vertData.push_back(Vertex(topRight,		glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)));
	//m_vertData.push_back(Vertex(bottomLeft,		glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)));

}

void Chunk::loadFacePositiveX(int x, int y, int z)
{
	glm::vec3 bottomLeft	(x * 2 + BLOCK_RENDER_SIZE, y * 2 - BLOCK_RENDER_SIZE, z * 2 + BLOCK_RENDER_SIZE);
	glm::vec3 topLeft		(x * 2 + BLOCK_RENDER_SIZE, y * 2 + BLOCK_RENDER_SIZE, z * 2 + BLOCK_RENDER_SIZE);
	glm::vec3 bottomRight	(x * 2 + BLOCK_RENDER_SIZE, y * 2 - BLOCK_RENDER_SIZE, z * 2 - BLOCK_RENDER_SIZE);
	glm::vec3 topRight		(x * 2 + BLOCK_RENDER_SIZE, y * 2 + BLOCK_RENDER_SIZE, z * 2 - BLOCK_RENDER_SIZE);

	uint32_t offset = m_vertDataAOS.size();
	m_vertIndices.emplace_back(offset + 0);
	m_vertIndices.emplace_back(offset + 1);
	m_vertIndices.emplace_back(offset + 2);
	m_vertIndices.emplace_back(offset + 2);
	m_vertIndices.emplace_back(offset + 3);
	m_vertIndices.emplace_back(offset + 0);

	m_vertDataAOS.push_back(Vertex(topRight, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)));
	m_vertDataAOS.push_back(Vertex(bottomRight, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)));
	m_vertDataAOS.push_back(Vertex(bottomLeft, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)));
	m_vertDataAOS.push_back(Vertex(topLeft,		glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)));
	//m_vertData.push_back(Vertex(topRight,		glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)));
	//m_vertData.push_back(Vertex(bottomLeft,		glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)));
}

void Chunk::loadFaceNegativeY(int x, int y, int z)
{
	glm::vec3 topLeft(x * 2 - BLOCK_RENDER_SIZE, y * 2 - BLOCK_RENDER_SIZE, z * 2 + BLOCK_RENDER_SIZE);
	glm::vec3 topRight(x * 2 + BLOCK_RENDER_SIZE, y * 2 - BLOCK_RENDER_SIZE, z * 2 + BLOCK_RENDER_SIZE);
	glm::vec3 bottomRight(x * 2 + BLOCK_RENDER_SIZE, y * 2 - BLOCK_RENDER_SIZE, z * 2 - BLOCK_RENDER_SIZE);
	glm::vec3 bottomLeft(x * 2 - BLOCK_RENDER_SIZE, y * 2 - BLOCK_RENDER_SIZE, z * 2 - BLOCK_RENDER_SIZE);

	uint32_t offset = m_vertDataAOS.size();
	m_vertIndices.emplace_back(offset + 0);
	m_vertIndices.emplace_back(offset + 1);
	m_vertIndices.emplace_back(offset + 2);
	m_vertIndices.emplace_back(offset + 2);
	m_vertIndices.emplace_back(offset + 3);
	m_vertIndices.emplace_back(offset + 0);

	m_vertDataAOS.push_back(Vertex(topRight, glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f)));
	m_vertDataAOS.push_back(Vertex(bottomRight, glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f)));
	m_vertDataAOS.push_back(Vertex(bottomLeft, glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f)));
	m_vertDataAOS.push_back(Vertex(topLeft,		glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f)));
	//m_vertData.push_back(Vertex(topRight,		glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f)));
	//m_vertData.push_back(Vertex(bottomLeft,		glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f)));
}

void Chunk::loadFacePositiveY(int x, int y, int z)
{
	glm::vec3 bottomRight	(x * 2 + BLOCK_RENDER_SIZE, y * 2 + BLOCK_RENDER_SIZE, z * 2 + BLOCK_RENDER_SIZE);
	glm::vec3 bottomLeft	(x * 2 - BLOCK_RENDER_SIZE, y * 2 + BLOCK_RENDER_SIZE, z * 2 + BLOCK_RENDER_SIZE);
	glm::vec3 topLeft		(x * 2 - BLOCK_RENDER_SIZE, y * 2 + BLOCK_RENDER_SIZE, z * 2 - BLOCK_RENDER_SIZE);
	glm::vec3 topRight		(x * 2 + BLOCK_RENDER_SIZE, y * 2 + BLOCK_RENDER_SIZE, z * 2 - BLOCK_RENDER_SIZE);

	uint32_t offset = m_vertDataAOS.size();
	m_vertIndices.emplace_back(offset + 0);
	m_vertIndices.emplace_back(offset + 1);
	m_vertIndices.emplace_back(offset + 2);
	m_vertIndices.emplace_back(offset + 2);
	m_vertIndices.emplace_back(offset + 3);
	m_vertIndices.emplace_back(offset + 0);

	m_vertDataAOS.push_back(Vertex(topRight, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f)));
	m_vertDataAOS.push_back(Vertex(bottomRight, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f)));
	m_vertDataAOS.push_back(Vertex(bottomLeft, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f)));
	m_vertDataAOS.push_back(Vertex(topLeft,		glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f)));
	//m_vertData.push_back(Vertex(topRight,		glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f)));
	//m_vertData.push_back(Vertex(bottomLeft,		glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f)));
}

void Chunk::loadFaceNegativeZ(int x, int y, int z)
{
	glm::vec3 bottomLeft	(x * 2 + BLOCK_RENDER_SIZE, y * 2 - BLOCK_RENDER_SIZE, z * 2 - BLOCK_RENDER_SIZE);
	glm::vec3 bottomRight	(x * 2 - BLOCK_RENDER_SIZE, y * 2 - BLOCK_RENDER_SIZE, z * 2 - BLOCK_RENDER_SIZE);
	glm::vec3 topRight		(x * 2 - BLOCK_RENDER_SIZE, y * 2 + BLOCK_RENDER_SIZE, z * 2 - BLOCK_RENDER_SIZE);
	glm::vec3 topLeft		(x * 2 + BLOCK_RENDER_SIZE, y * 2 + BLOCK_RENDER_SIZE, z * 2 - BLOCK_RENDER_SIZE);

	uint32_t offset = m_vertDataAOS.size();
	m_vertIndices.emplace_back(offset + 0);
	m_vertIndices.emplace_back(offset + 1);
	m_vertIndices.emplace_back(offset + 2);
	m_vertIndices.emplace_back(offset + 2);
	m_vertIndices.emplace_back(offset + 3);
	m_vertIndices.emplace_back(offset + 0);

	m_vertDataAOS.push_back(Vertex(topRight, glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f)));
	m_vertDataAOS.push_back(Vertex(bottomRight, glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f)));
	m_vertDataAOS.push_back(Vertex(bottomLeft, glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f)));
	m_vertDataAOS.push_back(Vertex(topLeft,		glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f)));
	//m_vertData.push_back(Vertex(topRight,		glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f)));
	//m_vertData.push_back(Vertex(bottomLeft,		glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f)));
}

void Chunk::loadFacePositiveZ(int x, int y, int z)
{
	glm::vec3 bottomLeft	(x * 2 - BLOCK_RENDER_SIZE, y * 2 - BLOCK_RENDER_SIZE, z * 2 + BLOCK_RENDER_SIZE);
	glm::vec3 bottomRight	(x * 2 + BLOCK_RENDER_SIZE, y * 2 - BLOCK_RENDER_SIZE, z * 2 + BLOCK_RENDER_SIZE);
	glm::vec3 topRight		(x * 2 + BLOCK_RENDER_SIZE, y * 2 + BLOCK_RENDER_SIZE, z * 2 + BLOCK_RENDER_SIZE);
	glm::vec3 topLeft		(x * 2 - BLOCK_RENDER_SIZE, y * 2 + BLOCK_RENDER_SIZE, z * 2 + BLOCK_RENDER_SIZE);

	uint32_t offset = m_vertDataAOS.size();
	m_vertIndices.emplace_back(offset + 0);
	m_vertIndices.emplace_back(offset + 1);
	m_vertIndices.emplace_back(offset + 2);
	m_vertIndices.emplace_back(offset + 2);
	m_vertIndices.emplace_back(offset + 3);
	m_vertIndices.emplace_back(offset + 0);

	m_vertDataAOS.push_back(Vertex(topRight,		glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f)));
	m_vertDataAOS.push_back(Vertex(bottomRight,	glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f)));
	m_vertDataAOS.push_back(Vertex(bottomLeft,		glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f)));
	m_vertDataAOS.push_back(Vertex(topLeft,		glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f)));
	//m_vertData.push_back(Vertex(topRight,		glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f)));
	//m_vertData.push_back(Vertex(bottomLeft,		glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f)));
}
