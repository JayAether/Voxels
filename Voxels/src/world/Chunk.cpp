#include "Chunk.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/noise.hpp>

#include "voxels/Block.h"
#include "../engine/graphics/buffers/VertexBuffer.h"

#include "../3rdparty/FastNoiseLite.h"

#include <vector>
#include <list>


Chunk::Chunk(CHUNK_OFFSET chunkoffset)
	: m_chunkOffset(chunkoffset)
{
	glGenVertexArrays(1, &m_VAO);
	m_vb = VertexBuffer();

	m_blocks.reserve(CHUNK_VOL);

	for (int i = 0; i < CHUNK_VOL; i++)
	{
		//Block block;
		m_blocks.push_back(Block());
	}

	generateMap();

	

}

Chunk::~Chunk()
{
	glDeleteVertexArrays(1, &m_VAO);

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
	//int waterLevel = 10;
	//int heightExtremification = 2;
	////float dampner = 200.0f;
	//FastNoiseLite noise;

	//CHUNK_OFFSET trueChunkPos = m_chunkOffset * CHUNK_LEN;


	//for (int x = 0; x < CHUNK_LEN; x++) // block in chunk cycle
	//{
	//	{
	//		for (int z = 0; z < CHUNK_LEN; z++) // block in chunk cycle
	//		{
	//			glm::vec3 trueBlockPos = glm::vec3(x, 0, z);

	//			trueBlockPos += trueChunkPos;

	//			float height = noise.GetNoise(trueBlockPos.x, trueBlockPos.z) * CHUNK_LEN;
	//			//height *= heightExtremification;



	//			for (int y = 0; y < CHUNK_LEN; y++) // block in chunk cycle
	//			{
	//				if (y > height)
	//				{
	//					setBlock(STONE_BLOCK, x, y, z);
	//				}
	//			}

	//		}
	//	}
	//}
	//return;
	//float dampner = 1.0f;
	////float dampner = 200.0f;
	//FastNoiseLite noise;

	//glm::ivec3 chunkPos = m_chunkOffset * CHUNK_LEN;

	//for (int x = 0; x < CHUNK_LEN; x++)
	//{
	//	for (int z = 0; z < CHUNK_LEN; z++)
	//	{
	//		glm::vec2 v = glm::vec2(x + chunkPos.x, z + chunkPos.z);

	//		//float height = glm::abs(glm::simplex(v)) * CHUNK_LEN;
	//		float height = noise.GetNoise(v.x, v.y) * CHUNK_LEN;
	//		height *= 2;

	//		int i = (int) round(glfwGetTime());

	//		for (int y = 0; y < CHUNK_LEN; y++)
	//		{

	//			if (chunkPos.y + 0 > height)
	//			{
	//				setBlock(COLORED_BLOCK, x, y, z);
	//			}
	//			/*printf("fuck me %i %i", i, y);
	//			i += 10;*/
	//			if (y == 124);
	//			{
	//				//i++;
	//			}
	//		}
	//	}
	//}

	glm::vec3 chunkPos; 
	chunkPos = m_chunkOffset * CHUNK_LEN;

	FastNoiseLite noise;
	float chunkHeightMap[CHUNK_AREA];

	for (int x = 0; x < CHUNK_LEN; x++)
	{
		for (int z = 0; z < CHUNK_LEN; z++)
		{
			int index = getBlockIndex2D(x, z);
			//chunkHeightMap->push_back(noise.GetNoise(x + chunkPos.x, z + chunkPos.z) *CHUNK_LEN);
			//chunkHeightMap->at(index) = noise.GetNoise(x + chunkPos.x, z + chunkPos.z);
			chunkHeightMap[index] = noise.GetNoise(x + chunkPos.x, z + chunkPos.z)*CHUNK_LEN;
			//chunkHeightMap->insert(index, noise.GetNoise(x + chunkPos.x, z + chunkPos.z));

		}
	}



	for (int x = 0; x < CHUNK_LEN; x++)
	{

		for (int z = 0; z < CHUNK_LEN; z++)
		{
			int index = getBlockIndex2D(x, z);

			for (int y = 0; y < CHUNK_LEN; y++)
			{

				if (chunkHeightMap[index] < y)
				{
					setBlock(COLORED_BLOCK, x, y, z);
				}
			}
		}
	}

}

void Chunk::bakeCore()
{
	bool shouldDraw = false;
	for (int x = 0; x < CHUNK_LEN; x++)
	{
		for (int y = 0; y < CHUNK_LEN; y++)
		{
			for (int z = 0; z < CHUNK_LEN; z++)
			{
				shouldDraw = false;

				if (m_blocks.at(getBlockIndex3D(x, y, z)).isActive() == false) // skip inactive
				{
					continue;
				}

				if (x == 0 || x == CHUNK_LEN - 1 || y == 0 || y == CHUNK_LEN - 1 || z == 0 || z == CHUNK_LEN - 1)
				{
					shouldDraw = true; // this renders the chunk borders
					// this is unnecessary in general but im doing rn to see progress
				}
				else
				{
					shouldDraw =
						((!m_blocks.at(getBlockIndex3D(x - 1, y, z)).isSolidBlock()) ||
							(!m_blocks.at(getBlockIndex3D(x + 1, y, z)).isSolidBlock()) ||
							(!m_blocks.at(getBlockIndex3D(x, y - 1, z)).isSolidBlock()) ||
							(!m_blocks.at(getBlockIndex3D(x, y + 1, z)).isSolidBlock()) ||
							(!m_blocks.at(getBlockIndex3D(x, y, z - 1)).isSolidBlock()) ||
							(!m_blocks.at(getBlockIndex3D(x, y, z + 1)).isSolidBlock())); // this here makes it so the blocks render if even just 1 face is visable 
					// should prob make a way to get rid of extra hidden faces, maybe send the visable faces to the vert shader and only render the ones that are
				}

				if (shouldDraw && (m_blocks.at(getBlockIndex3D(x, y, z)).isSolidBlock()))
				{
					createCoreBlock(x, y, z);
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
		int x = 0;

		for (int y = 0; y < CHUNK_LEN; y++)
		{
			for (int z = 0; z < CHUNK_LEN; z++)
			{
				int index = getBlockIndex3D(x, y, z);

				//bool state = blockStates->at(getBlowckIndex2D(y, z));
				if (m_blocks.at(index).isSolidBlock() && !blockStates[getBlockIndex2D(y, z)])
					createBorderBlock(direction, x, y, z);
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
				if (m_blocks.at(index).isSolidBlock() && !blockStates[getBlockIndex2D(y, z)])
					createBorderBlock(direction, x, y, z);
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
				if (m_blocks.at(index).isSolidBlock() && !blockStates[getBlockIndex2D(x, z)])
					createBorderBlock(direction, x, y, z);
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
				if (m_blocks.at(index).isSolidBlock() && !blockStates[getBlockIndex2D(x, z)])
					createBorderBlock(direction, x, y, z);
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
				if (m_blocks.at(index).isSolidBlock() && !blockStates[getBlockIndex2D(x, y)])
					createBorderBlock(direction, x, y, z);
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
				if (m_blocks.at(index).isSolidBlock() && !blockStates[getBlockIndex2D(x, y)])
					createBorderBlock(direction, x, y, z);
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

	return (
		(!m_blocks.at(getBlockIndex3D(x - 1, y, z)).isSolidBlock()) ||
		(!m_blocks.at(getBlockIndex3D(x + 1, y, z)).isSolidBlock()) ||
		(!m_blocks.at(getBlockIndex3D(x, y - 1, z)).isSolidBlock()) ||
		(!m_blocks.at(getBlockIndex3D(x, y + 1, z)).isSolidBlock()) ||
		(!m_blocks.at(getBlockIndex3D(x, y, z - 1)).isSolidBlock()) ||
		(!m_blocks.at(getBlockIndex3D(x, y, z + 1)).isSolidBlock())
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

					blockStates[index2D] = m_blocks.at(index3D).isSolidBlock();
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

					blockStates[index2D] = m_blocks.at(index3D).isSolidBlock();
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

					blockStates[index2D] = m_blocks.at(index3D).isSolidBlock();
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

					blockStates[index2D] = m_blocks.at(index3D).isSolidBlock();
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

					blockStates[index2D] = m_blocks.at(index3D).isSolidBlock();
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

					blockStates[index2D] = m_blocks.at(index3D).isSolidBlock();
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

	m_vb.Bind();
	m_vb.setData(&m_data[0], sizeof(m_data[0]) * m_data.size());

	glEnableVertexAttribArray(POSITION_LOCATION);
	glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(1 * sizeof(glm::vec3)));

	//glEnableVertexAttribArray(2);
	//glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2 * sizeof(glm::vec3)));

	//glEnableVertexAttribArray(3);
	//glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(glm::vec3)));

	m_vb.Unbind();
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

void Chunk::setBlock(BlockID blockType, int x, int y, int z)
{
	int index = getBlockIndex3D(x, y, z);
	m_blocks.at(index).changeType(blockType);
}

void Chunk::createCoreBlock(int x, int y, int z)
{
	// neg x
	if (x - 1 >= 0 && !m_blocks.at(getBlockIndex3D(x - 1, y, z)).isSolidBlock())
	{
		loadFaceNegativeX(x, y, z);
	}

	// pos x
	if (x + 1 < CHUNK_LEN && !m_blocks.at(getBlockIndex3D(x + 1, y, z)).isSolidBlock())
	{
		loadFacePositiveX(x, y, z);
	}

	// neg y
	if (y - 1 >= 0 && !m_blocks.at(getBlockIndex3D(x, y - 1, z)).isSolidBlock())
	{
		loadFaceNegativeY(x, y, z);
	}

	// pos y
	if (y + 1 < CHUNK_LEN && !m_blocks.at(getBlockIndex3D(x, y + 1, z)).isSolidBlock())
	{
		loadFacePositiveY(x, y, z);
	}

	// neg z
	if (z-1 >= 0 && !m_blocks.at(getBlockIndex3D(x, y, z - 1)).isSolidBlock())
	{
		loadFaceNegativeZ(x, y, z);
	}

	// pos z
	if (z+1 < CHUNK_LEN && !m_blocks.at(getBlockIndex3D(x, y, z + 1)).isSolidBlock())
	{
		loadFacePositiveZ(x, y, z);
	}
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
