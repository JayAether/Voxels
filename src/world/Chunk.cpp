#include "Chunk.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/noise.hpp>

#include "voxels/Block.h"

#include "../3rdparty/FastNoiseLite.h"

#include <vector>
#include <list>


Chunk::Chunk(CHUNK_OFFSET chunkoffset)
	: m_chunkOffset(chunkoffset)
{
	glCreateVertexArrays(1, &m_VAO);
	glCreateBuffers(1, &m_vb);



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

	// TODO: actualy delete the blocks

}

void Chunk::render(ShaderProgram program)
{
	if (m_data.size() == 0) // this exists so opengl doesnt freakout if chunk is empty
	{
		return;
	}

	program.use();

	program.setValue("chunkOffset", m_chunkOffset);
	program.setValue("chunkSize", CHUNK_LEN);
	glm::mat4 model = glm::mat4(1.0f);
	program.setValue("model", model);

	glBindVertexArray(m_VAO);

	glDrawArrays(GL_TRIANGLES, 0, sizeof(m_data[0]) * m_data.size()); // should change the `sizeof(m_data[0]) * m_data.size()` to some sort of variable that keeps track of rendered blocks
	glBindVertexArray(0);	
}

void Chunk::generateMap()
{


	glm::vec3 chunkPos; 
	chunkPos = m_chunkOffset * CHUNK_LEN;

	FastNoiseLite noise;
	float chunkHeightMap[CHUNK_AREA];

	for (uint32_t x = 0; x < CHUNK_LEN; x++)
	{
		for (uint32_t z = 0; z < CHUNK_LEN; z++)
		{
			uint32_t index = getBlockIndex2D(x, z);

			chunkHeightMap[index] = noise.GetNoise(x + chunkPos.x, z + chunkPos.z)* 15.0f + 17.0f;



		}
	}



	for (int x = 0; x < CHUNK_LEN; x++)
	{

		for (int z = 0; z < CHUNK_LEN; z++)
		{
			int index = getBlockIndex2D(x, z);

			// TODO check this in ddebug

			//std::bitset<CHUNK_LEN> buf = (~0) << (uint32_t)chunkHeightMap[index];
			//m_active.at(x).at(z) = buf >> (uint32_t)chunkHeightMap[index];

			for (uint32_t y = 0; y < CHUNK_LEN; y++)
			{
				if (y < chunkHeightMap[index])
					m_active.at(x).at(z)[y] = 1;
			}

			//for (uint32_t y = 0; y < CHUNK_LEN; y++)
				//m_active.at(x).at(z)[y] = 1;

		}
	}

}

void Chunk::bakeCore()
{
	

	for (uint32_t x = 0; x < CHUNK_LEN; x++)
	{
		for (uint32_t y = 0; y < CHUNK_LEN; y++)
		{
			for (uint32_t z = 0; z < CHUNK_LEN; z++)
			{
				bool shouldDraw = false;


				// skip inactive
				if (m_active.at(x).at(z)[y] == 0)
					continue;

				// draw if core
				if (x > 0 && x < CHUNK_LEN - 1 && z > 0 && z < CHUNK_LEN - 1 && y > 0 && y < CHUNK_LEN - 1)
					if (m_active.at(x - 1).at(z)[y] == 0 || 
						m_active.at(x + 1).at(z)[y] == 0 ||
						m_active.at(x).at(z)[y - 1] == 0 ||
						m_active.at(x).at(z)[y + 1] == 0 ||
						m_active.at(x).at(z - 1)[y] == 0 ||
						m_active.at(x).at(z + 1)[y] == 0)
							createCoreBlock(x, y, z);


				if (shouldDraw)
				{
				}
			}
		}
	}
}

void Chunk::bakeBorder(Faces direction, bool* blockStates)
{
	// the 'direction' is which direction the data is applied to




	switch (direction)
	{
	case NEGATIVE_X:
	{
		uint32_t x = 0;

		for (uint32_t y = 0; y < CHUNK_LEN; y++)
		{
			for (uint32_t z = 0; z < CHUNK_LEN; z++)
			{
				uint32_t index = getBlockIndex3D(x, y, z);

				//bool state = blockStates->at(getBlowckIndex2D(y, z));
				if (m_active.at(x).at(z)[y] == 1)
				{
					loadFaceNegativeX(x, y, z);
					loadFacePositiveX(x, y, z);
					loadFaceNegativeY(x, y, z);
					loadFacePositiveY(x, y, z);
					loadFaceNegativeZ(x, y, z);
					loadFacePositiveZ(x, y, z);
				}
					//createBorderBlock(direction, x, y, z);
			}
		}

		break;
	}
	case POSITIVE_X:
	{
		int x = CHUNK_LEN - 1;

		for (int y = 0; y < CHUNK_LEN; y++)
		{
			for (int z = 0; z < CHUNK_LEN; z++)
			{
				int index = getBlockIndex3D(x, y, z);

				//bool state = blockStates->at(getBlowckIndex2D(y, z));
				if (m_active.at(x).at(z)[y] == 1)
				{
					loadFaceNegativeX(x, y, z);
					loadFacePositiveX(x, y, z);
					loadFaceNegativeY(x, y, z);
					loadFacePositiveY(x, y, z);
					loadFaceNegativeZ(x, y, z);
					loadFacePositiveZ(x, y, z);
				}
					//createBorderBlock(direction, x, y, z);
			}
		}

		break;
	}
	case NEGATIVE_Y:
	{
		int y = 0;

		for (int x = 0; x < CHUNK_LEN; x++)
		{
			for (int z = 0; z < CHUNK_LEN; z++)
			{
				int index = getBlockIndex3D(x, y, z);

				//bool state = blockStates->at(getBlowckIndex2D(y, z));
				if (m_active.at(x).at(z)[y] == 0)
				{
					loadFaceNegativeX(x, y, z);
					loadFacePositiveX(x, y, z);
					loadFaceNegativeY(x, y, z);
					loadFacePositiveY(x, y, z);
					loadFaceNegativeZ(x, y, z);
					loadFacePositiveZ(x, y, z);
				}
					//createBorderBlock(direction, x, y, z);
			}
		}

		break;
	}
	case POSITIVE_Y:
	{
		int y = CHUNK_LEN - 1;

		for (int x = 0; x < CHUNK_LEN; x++)
		{
			for (int z = 0; z < CHUNK_LEN; z++)
			{
				int index = getBlockIndex3D(x, y, z);

				//bool state = blockStates->at(getBlowckIndex2D(y, z));
				if (m_active.at(x).at(z)[y] == 0)
				{
					loadFaceNegativeX(x, y, z);
					loadFacePositiveX(x, y, z);
					loadFaceNegativeY(x, y, z);
					loadFacePositiveY(x, y, z);
					loadFaceNegativeZ(x, y, z);
					loadFacePositiveZ(x, y, z);
				}
					//createBorderBlock(direction, x, y, z);
			}
		}

		break;
	}
	case NEGATIVE_Z:
	{
		int z = 0;

		for (int x = 0; x < CHUNK_LEN; x++)
		{
			for (int y = 0; y < CHUNK_LEN; y++)
			{
				int index = getBlockIndex3D(x, y, z);

				//bool state = blockStates->at(getBlowckIndex2D(y, z));
				if (m_active.at(x).at(z)[y] == 1)
				{
					loadFaceNegativeX(x, y, z);
					loadFacePositiveX(x, y, z);
					loadFaceNegativeY(x, y, z);
					loadFacePositiveY(x, y, z);
					loadFaceNegativeZ(x, y, z);
					loadFacePositiveZ(x, y, z);
				}
					//createBorderBlock(direction, x, y, z);
			}
		}

		break;
	}
	case POSITIVE_Z:
	{
		int z = CHUNK_LEN - 1;

		for (int x = 0; x < CHUNK_LEN; x++)
		{
			for (int y = 0; y < CHUNK_LEN; y++)
			{
				int index = getBlockIndex3D(x, y, z);

				//bool state = blockStates->at(getBlowckIndex2D(y, z));
				if (m_active.at(x).at(z)[y] == 1)
				{
					loadFaceNegativeX(x, y, z);
					loadFacePositiveX(x, y, z);
					loadFaceNegativeY(x, y, z);
					loadFacePositiveY(x, y, z);
					loadFaceNegativeZ(x, y, z);
					loadFacePositiveZ(x, y, z);
				}
					//createBorderBlock(direction, x, y, z);
			}
		}

		break;
	}
	default:
		break;
	}
}

bool Chunk::coreBlockHasExposedFaces(int x, int y, int z) const
{
	if (x == 0 || x == CHUNK_LEN - 1 || y == 0 || y == CHUNK_LEN - 1 || z == 0 || z == CHUNK_LEN - 1)
		return true;

	//return (
	//	(!m_blocks.at(getBlockIndex3D(x - 1, y, z)).isSolidBlock()) ||
	//	(!m_blocks.at(getBlockIndex3D(x + 1, y, z)).isSolidBlock()) ||
	//	(!m_blocks.at(getBlockIndex3D(x, y - 1, z)).isSolidBlock()) ||
	//	(!m_blocks.at(getBlockIndex3D(x, y + 1, z)).isSolidBlock()) ||
	//	(!m_blocks.at(getBlockIndex3D(x, y, z - 1)).isSolidBlock()) ||
	//	(!m_blocks.at(getBlockIndex3D(x, y, z + 1)).isSolidBlock())
	//	);
	return (
		(m_active.at(x - 1).at(z)[y] == 0) ||
		(m_active.at(x + 1).at(z)[y] == 0) ||
		(m_active.at(x).at(z)[y - 1] == 0) ||
		(m_active.at(x).at(z)[y + 1] == 0) ||
		(m_active.at(x).at(z - 1)[y] == 0) ||
		(m_active.at(x).at(z + 1)[y] == 0)
		);
}

void Chunk::getBorderBlockStates(Faces direction, bool* blockStates) const
{
	// the 'direction' is which direction the data should be pulled from

	switch (direction)
	{
		case Faces::NEGATIVE_X:
		{
			int x = 0;

			for (int y = 0; y < CHUNK_LEN; y++)
			{
				for (int z = 0; z < CHUNK_LEN; z++)
				{
					int index3D = getBlockIndex3D(x, y, z);
					int index2D = getBlockIndex2D(y, z);

					blockStates[index2D] = m_active.at(x).at(z)[y];
				}
			}

			break;
		}
		case Faces::POSITIVE_X:
		{
			int x = CHUNK_LEN - 1;

			for (int y = 0; y < CHUNK_LEN; y++)
			{
				for (int z = 0; z < CHUNK_LEN; z++)
				{
					int index3D = getBlockIndex3D(x, y, z);
					int index2D = getBlockIndex2D(y, z);

					blockStates[index2D] = m_active.at(x).at(z)[y];
				}
			}

			break;
		}
		case Faces::NEGATIVE_Y:
		{
			int y = 0;

			for (int x = 0; x < CHUNK_LEN; x++)
			{
				for (int z = 0; z < CHUNK_LEN; z++)
				{
					int index3D = getBlockIndex3D(x, y, z);
					int index2D = getBlockIndex2D(x, z);

					blockStates[index2D] = m_active.at(x).at(z)[y];
				}
			}
			break;
		}
		case Faces::POSITIVE_Y:
		{
			int y = CHUNK_LEN - 1;

			for (int x = 0; x < CHUNK_LEN; x++)
			{
				for (int z = 0; z < CHUNK_LEN; z++)
				{
					int index3D = getBlockIndex3D(x, y, z);
					int index2D = getBlockIndex2D(x, z);

					blockStates[index2D] = m_active.at(x).at(z)[y];
				}
			}
			break;
		}
		case Faces::NEGATIVE_Z:
		{
			int z = 0;

			for (int x = 0; x < CHUNK_LEN; x++)
			{
				for (int y = 0; y < CHUNK_LEN; y++)
				{
					int index3D = getBlockIndex3D(x, y, z);
					int index2D = getBlockIndex2D(x, y);

					blockStates[index2D] = m_active.at(x).at(z)[y];
				}
			}

			break;
		}
		case Faces::POSITIVE_Z:
		{
			int z = CHUNK_LEN - 1;

			for (int x = 0; x < CHUNK_LEN; x++)
			{
				for (int y = 0; y < CHUNK_LEN; y++)
				{
					int index3D = getBlockIndex3D(x, y, z);
					int index2D = getBlockIndex2D(x, y);

					blockStates[index2D] = m_active.at(x).at(z)[y];
				}
			}

			break;
		}
		default:
			break;
	}
}

void Chunk::clearData()
{
	m_data.clear();
}

void Chunk::populateBuffers()
{
	if (m_data.size() == 0) // this exists so opengl doesnt freakout if chunk is empty
	{
		return;
	}

	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_vb);
	//m_vb.setData(&m_data[0], sizeof(m_data[0]) * m_data.size());

	//glNamedBufferStorage(m_vb[POS_BUFFER],)
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_data[0]) * m_data.size(), m_data.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(POSITION_LOCATION);
	glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(1 * sizeof(glm::vec3)));

	//glEnableVertexAttribArray(2);
	//glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2 * sizeof(glm::vec3)));

	//glEnableVertexAttribArray(3);
	//glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(glm::vec3)));

	//m_vb.Unbind();
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

CHUNK_OFFSET Chunk::getChunkOffset() const
{
	return m_chunkOffset;
}

int Chunk::getBlockIndex3D(int x, int y, int z) const
{
	return (x) + (y * CHUNK_LEN) + (z * CHUNK_AREA);
}

int Chunk::getBlockIndex2D(int x, int y) const
{
	return x + y * CHUNK_LEN;
}

//void Chunk::setBlock(BlockID blockType, int x, int y, int z)
//{
//	int index = getBlockIndex3D(x, y, z);
//	m_blocks.at(index).changeType(blockType);
//
//	//m_active.at(x).at(z)[y] 
//}

void Chunk::createCoreBlock(int x, int y, int z)
{
	if (x - 1 >= 0			&&	m_active.at(x - 1).at(z)[y] == 0) { loadFaceNegativeX(x, y, z); } // neg x
	if (x + 1 <	CHUNK_LEN	&&	m_active.at(x + 1).at(z)[y] == 0) { loadFacePositiveX(x, y, z); } // pos x
	if (y - 1 >= 0			&&	m_active.at(x).at(z)[y - 1] == 0) { loadFaceNegativeY(x, y, z); } // neg y
	if (y + 1 < CHUNK_LEN	&&	m_active.at(x).at(z)[y + 1] == 0) { loadFacePositiveY(x, y, z); } // pos y
	if (z - 1 >= 0			&&	m_active.at(x).at(z - 1)[y] == 0) { loadFaceNegativeZ(x, y, z); } // neg z
	if (z + 1 < CHUNK_LEN	&&	m_active.at(x).at(z + 1)[y] == 0) { loadFacePositiveZ(x, y, z); } // pos z
}

void Chunk::createBorderBlock(Faces direction, int x, int y, int z)
{
	switch (direction)
	{
	case NEGATIVE_X:
		loadFaceNegativeX(x, y, z);
		break;
	case POSITIVE_X:
		loadFacePositiveX(x, y, z);
		break;
	case NEGATIVE_Y:
		loadFaceNegativeY(x, y, z);
		break;
	case POSITIVE_Y:
		loadFacePositiveY(x, y, z);
		break;
	case NEGATIVE_Z:
		loadFaceNegativeZ(x, y, z);
		break;
	case POSITIVE_Z:
		loadFacePositiveZ(x, y, z);
		break;
	default:
		break;
	}
}

void Chunk::loadFaceNegativeX(int x, int y, int z)
{
	glm::vec3 bottomRight(x * 2 - Block::BLOCK_RENDER_SIZE, y * 2 - Block::BLOCK_RENDER_SIZE, z * 2 + Block::BLOCK_RENDER_SIZE);
	glm::vec3 topRight(x * 2 - Block::BLOCK_RENDER_SIZE, y * 2 + Block::BLOCK_RENDER_SIZE, z * 2 + Block::BLOCK_RENDER_SIZE);
	glm::vec3 bottomLeft(x * 2 - Block::BLOCK_RENDER_SIZE, y * 2 - Block::BLOCK_RENDER_SIZE, z * 2 - Block::BLOCK_RENDER_SIZE);
	glm::vec3 topLeft(x * 2 - Block::BLOCK_RENDER_SIZE, y * 2 + Block::BLOCK_RENDER_SIZE, z * 2 - Block::BLOCK_RENDER_SIZE);

	//Vertex v1 = Vertex(topLeft, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(0, 1));


	m_data.push_back(Vertex(bottomLeft, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(1, 1)));
	m_data.push_back(Vertex(topLeft, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(0, 1)));
	m_data.push_back(Vertex(topRight, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(x, y, z), glm::vec2(0, 0)));
	m_data.push_back(Vertex(topRight, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(x, y, z), glm::vec2(0, 0)));
	m_data.push_back(Vertex(bottomRight, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(1, 0)));
	m_data.push_back(Vertex(bottomLeft, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(1, 1)));
}

void Chunk::loadFacePositiveX(int x, int y, int z)
{
	glm::vec3 bottomLeft(x * 2 + Block::BLOCK_RENDER_SIZE, y * 2 - Block::BLOCK_RENDER_SIZE, z * 2 + Block::BLOCK_RENDER_SIZE);
	glm::vec3 topLeft(x * 2 + Block::BLOCK_RENDER_SIZE, y * 2 + Block::BLOCK_RENDER_SIZE, z * 2 + Block::BLOCK_RENDER_SIZE);
	glm::vec3 bottomRight(x * 2 + Block::BLOCK_RENDER_SIZE, y * 2 - Block::BLOCK_RENDER_SIZE, z * 2 - Block::BLOCK_RENDER_SIZE);
	glm::vec3 topRight(x * 2 + Block::BLOCK_RENDER_SIZE, y * 2 + Block::BLOCK_RENDER_SIZE, z * 2 - Block::BLOCK_RENDER_SIZE);

	m_data.push_back(Vertex(bottomLeft, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(1, 1)));
	m_data.push_back(Vertex(topLeft, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(1, 0)));
	m_data.push_back(Vertex(topRight, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(x, y, z), glm::vec2(0, 0)));
	m_data.push_back(Vertex(topRight, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(x, y, z), glm::vec2(0, 0)));
	m_data.push_back(Vertex(bottomRight, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(0, 1)));
	m_data.push_back(Vertex(bottomLeft, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(1, 1)));

}

void Chunk::loadFaceNegativeY(int x, int y, int z)
{
	glm::vec3 topLeft(x * 2 - Block::BLOCK_RENDER_SIZE, y * 2 - Block::BLOCK_RENDER_SIZE, z * 2 + Block::BLOCK_RENDER_SIZE);
	glm::vec3 topRight(x * 2 + Block::BLOCK_RENDER_SIZE, y * 2 - Block::BLOCK_RENDER_SIZE, z * 2 + Block::BLOCK_RENDER_SIZE);
	glm::vec3 bottomRight(x * 2 + Block::BLOCK_RENDER_SIZE, y * 2 - Block::BLOCK_RENDER_SIZE, z * 2 - Block::BLOCK_RENDER_SIZE);
	glm::vec3 bottomLeft(x * 2 - Block::BLOCK_RENDER_SIZE, y * 2 - Block::BLOCK_RENDER_SIZE, z * 2 - Block::BLOCK_RENDER_SIZE);

	m_data.push_back(Vertex(bottomLeft, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(0, 0)));
	m_data.push_back(Vertex(topLeft, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(1, 0)));
	m_data.push_back(Vertex(topRight, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(x, y, z), glm::vec2(1, 1)));
	m_data.push_back(Vertex(topRight, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(x, y, z), glm::vec2(1, 1)));
	m_data.push_back(Vertex(bottomRight, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(0, 1)));
	m_data.push_back(Vertex(bottomLeft, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(0, 0)));
}

void Chunk::loadFacePositiveY(int x, int y, int z)
{
	glm::vec3 bottomRight(x * 2 + Block::BLOCK_RENDER_SIZE, y * 2 + Block::BLOCK_RENDER_SIZE, z * 2 + Block::BLOCK_RENDER_SIZE);
	glm::vec3 bottomLeft(x * 2 - Block::BLOCK_RENDER_SIZE, y * 2 + Block::BLOCK_RENDER_SIZE, z * 2 + Block::BLOCK_RENDER_SIZE);
	glm::vec3 topLeft(x * 2 - Block::BLOCK_RENDER_SIZE, y * 2 + Block::BLOCK_RENDER_SIZE, z * 2 - Block::BLOCK_RENDER_SIZE);
	glm::vec3 topRight(x * 2 + Block::BLOCK_RENDER_SIZE, y * 2 + Block::BLOCK_RENDER_SIZE, z * 2 - Block::BLOCK_RENDER_SIZE);

	m_data.push_back(Vertex(bottomLeft, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(0, 0)));
	m_data.push_back(Vertex(topLeft, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(1, 0)));
	m_data.push_back(Vertex(topRight, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(x, y, z), glm::vec2(1, 1)));
	m_data.push_back(Vertex(topRight, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(x, y, z), glm::vec2(1, 1)));
	m_data.push_back(Vertex(bottomRight, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(0, 1)));
	m_data.push_back(Vertex(bottomLeft, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(0, 0)));

}

void Chunk::loadFaceNegativeZ(int x, int y, int z)
{
	glm::vec3 bottomLeft(x * 2 + Block::BLOCK_RENDER_SIZE, y * 2 - Block::BLOCK_RENDER_SIZE, z * 2 - Block::BLOCK_RENDER_SIZE);
	glm::vec3 bottomRight(x * 2 - Block::BLOCK_RENDER_SIZE, y * 2 - Block::BLOCK_RENDER_SIZE, z * 2 - Block::BLOCK_RENDER_SIZE);
	glm::vec3 topRight(x * 2 - Block::BLOCK_RENDER_SIZE, y * 2 + Block::BLOCK_RENDER_SIZE, z * 2 - Block::BLOCK_RENDER_SIZE);
	glm::vec3 topLeft(x * 2 + Block::BLOCK_RENDER_SIZE, y * 2 + Block::BLOCK_RENDER_SIZE, z * 2 - Block::BLOCK_RENDER_SIZE);

	m_data.push_back(Vertex(bottomLeft, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(0, 0)));
	m_data.push_back(Vertex(topLeft, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(1, 0)));
	m_data.push_back(Vertex(topRight, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(x, y, z), glm::vec2(1, 1)));
	m_data.push_back(Vertex(topRight, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(x, y, z), glm::vec2(1, 1)));
	m_data.push_back(Vertex(bottomRight, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(0, 1)));
	m_data.push_back(Vertex(bottomLeft, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(0, 0)));
}

void Chunk::loadFacePositiveZ(int x, int y, int z)
{
	glm::vec3 bottomLeft(x * 2 - Block::BLOCK_RENDER_SIZE, y * 2 - Block::BLOCK_RENDER_SIZE, z * 2 + Block::BLOCK_RENDER_SIZE);
	glm::vec3 bottomRight(x * 2 + Block::BLOCK_RENDER_SIZE, y * 2 - Block::BLOCK_RENDER_SIZE, z * 2 + Block::BLOCK_RENDER_SIZE);
	glm::vec3 topRight(x * 2 + Block::BLOCK_RENDER_SIZE, y * 2 + Block::BLOCK_RENDER_SIZE, z * 2 + Block::BLOCK_RENDER_SIZE);
	glm::vec3 topLeft(x * 2 - Block::BLOCK_RENDER_SIZE, y * 2 + Block::BLOCK_RENDER_SIZE, z * 2 + Block::BLOCK_RENDER_SIZE);

	m_data.push_back(Vertex(bottomLeft, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(0, 0)));
	m_data.push_back(Vertex(topLeft, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(0, 1)));
	m_data.push_back(Vertex(topRight, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(x, y, z), glm::vec2(1, 1)));
	m_data.push_back(Vertex(topRight, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(x, y, z), glm::vec2(1, 1)));
	m_data.push_back(Vertex(bottomRight, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(1, 0)));
	m_data.push_back(Vertex(bottomLeft, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(0, 0)));
}
