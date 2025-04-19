#include "Chunk.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/noise.hpp>

#include "voxels/Block.h"
#include "../3rdparty/FastNoiseLite.h"

#include <vector>


Chunk::Chunk(CHUNK_POS chunkPos)
{
	this->m_chunkPos = chunkPos;

	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &m_buffer);
	m_vb = VertexBuffer();

	reserveBlockData();

	generateMap();

	//bake();
	//bakeCore();
}

Chunk::~Chunk()
{
	glDeleteVertexArrays(1, &m_VAO);
	glDeleteBuffers(1, &m_buffer);
}

//void Chunk::generateVoxels(int x, int y, int z, BlockID blockType)
//{
//
//}

void Chunk::update(float dt) 
{
	//bake();

}

void Chunk::render(ShaderProgram program)
{
	if (m_vertices.size() == 0) // this exists so opengl doesnt freakout if chunk is empty
	{
		return;
	}

	program.use();

	program.setValue("chunkOffset", m_chunkPos);
	program.setValue("chunkSize", CHUNK_LEN);
	glm::mat4 model = glm::mat4(1.0f);
	program.setValue("model", model);

	glBindVertexArray(m_VAO);

	glDrawArrays(GL_TRIANGLES, 0, sizeof(m_vertices[0]) * m_vertices.size()); // should change the `sizeof(m_vertices[0]) * m_vertices.size()` to some sort of variable that keeps track of rendered blocks
	glBindVertexArray(0);	
}


void Chunk::reserveBlockData()
{
	m_blocks.reserve(CHUNK_VOL);

	for (int i = 0; i < CHUNK_VOL; i++)
	{
		//Block block;
		m_blocks.push_back(Block());
	}
}

void Chunk::clearBufferData()
{
	m_vertices.clear();
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
		int x;
		int y;
		int z;

	case NEGATIVE_X:
		x = 0;

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
	case POSITIVE_X:
		x = CHUNK_LEN - 1;

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
	case NEGATIVE_Y:
		y = 0;

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
	case POSITIVE_Y:
		y = CHUNK_LEN - 1;

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
	case NEGATIVE_Z:
		z = 0;

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
	case POSITIVE_Z:
		z = CHUNK_LEN - 1;

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
	default:
		break;
	}
}


void Chunk::setBlock(BlockID blockType, int x, int y, int z)
{
	int index = getBlockIndex3D(x, y, z);
	m_blocks.at(index).changeType(blockType);
}

int Chunk::getBlockIndex3D(int x, int y, int z) 
{
	return x + y * CHUNK_LEN + z * CHUNK_AREA;
}

int Chunk::getBlockIndex2D(int x, int y)
{
	return x + y * CHUNK_LEN;
}


void Chunk::generateMap()
{
	float dampner = 1.0f;
	//float dampner = 200.0f;
	FastNoiseLite noise;

	for (int x = 0; x < CHUNK_LEN; x++)
	{
		for (int z = 0; z < CHUNK_LEN; z++)
		{
			glm::vec2 v = glm::vec2((x + m_chunkPos.x * CHUNK_LEN) / dampner, (z + m_chunkPos.z * CHUNK_LEN) / dampner);

			//float height = glm::abs(glm::simplex(v)) * CHUNK_LEN;
			float height = noise.GetNoise(v.x, v.y) * CHUNK_LEN;

			for (int y = 0; y < CHUNK_LEN; y++)
			{
				if (y > height)
				{
					setBlock(DEFAULT_AIR_BLOCK, x, y, z);
				}
			}
		}
	}
}

void Chunk::createCoreBlock(int x, int y, int z)
{
	glm::vec3 p1(x - Block::BLOCK_RENDER_SIZE, y - Block::BLOCK_RENDER_SIZE, z + Block::BLOCK_RENDER_SIZE);
	glm::vec3 p2(x + Block::BLOCK_RENDER_SIZE, y - Block::BLOCK_RENDER_SIZE, z + Block::BLOCK_RENDER_SIZE);
	glm::vec3 p3(x + Block::BLOCK_RENDER_SIZE, y + Block::BLOCK_RENDER_SIZE, z + Block::BLOCK_RENDER_SIZE);
	glm::vec3 p4(x - Block::BLOCK_RENDER_SIZE, y + Block::BLOCK_RENDER_SIZE, z + Block::BLOCK_RENDER_SIZE);
	glm::vec3 p5(x + Block::BLOCK_RENDER_SIZE, y - Block::BLOCK_RENDER_SIZE, z - Block::BLOCK_RENDER_SIZE);
	glm::vec3 p6(x - Block::BLOCK_RENDER_SIZE, y - Block::BLOCK_RENDER_SIZE, z - Block::BLOCK_RENDER_SIZE);
	glm::vec3 p7(x - Block::BLOCK_RENDER_SIZE, y + Block::BLOCK_RENDER_SIZE, z - Block::BLOCK_RENDER_SIZE);
	glm::vec3 p8(x + Block::BLOCK_RENDER_SIZE, y + Block::BLOCK_RENDER_SIZE, z - Block::BLOCK_RENDER_SIZE);

	/*glm::vec3 normal1 = glm::cross(p5 - p2, p8 - p2); //  x face
	glm::vec3 normal1 = glm::cross(p8 - p2, p3 - p1); //  x face
	glm::vec3 normal1 = glm::cross(p6 - p1, p7 - p2); // -x face
	glm::vec3 normal1 = glm::cross(p7 - p2, p4 - p2); // -x face

	glm::vec3 normal1 = glm::cross(p8 - p3, p7 - p3); //  y face
	glm::vec3 normal1 = glm::cross(p7 - p3, p4 - p3); //  y face
	glm::vec3 normal1 = glm::cross(p5 - p2, p6 - p2); // -y face
	glm::vec3 normal1 = glm::cross(p6 - p2, p1 - p2); // -y face

	glm::vec3 normal1 = glm::cross(p2 - p1, p3 - p1); //  z face
	glm::vec3 normal2 = glm::cross(p3 - p1, p4 - p1); //  z face
	glm::vec3 normal3 = glm::cross(p6 - p5, p6 - p7); // -z face
	glm::vec3 normal4 = glm::cross(p6 - p7, p6 - p8); // -z face*/

	//if (x == 0 || x == CHUNK_LEN - 1 || y == 0 || y == CHUNK_LEN - 1)
	//{
	//	return;
	//}


	// neg x
	if (x - 1 >= 0 && !m_blocks.at(getBlockIndex3D(x - 1, y, z)).isSolidBlock())
	{
		m_vertices.push_back(Vertex(p4, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(1, 1)));
		m_vertices.push_back(Vertex(p1, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(0, 1)));
		m_vertices.push_back(Vertex(p6, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(x, y, z), glm::vec2(0, 0)));
		m_vertices.push_back(Vertex(p6, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(x, y, z), glm::vec2(0, 0)));
		m_vertices.push_back(Vertex(p7, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(1, 0)));
		m_vertices.push_back(Vertex(p4, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(1, 1)));
	}

	// pos x
	if (x + 1 < CHUNK_LEN && !m_blocks.at(getBlockIndex3D(x + 1, y, z)).isSolidBlock())
	{
		m_vertices.push_back(Vertex(p3, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(1, 1)));
		m_vertices.push_back(Vertex(p8, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(1, 0)));
		m_vertices.push_back(Vertex(p5, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(x, y, z), glm::vec2(0, 0)));
		m_vertices.push_back(Vertex(p5, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(x, y, z), glm::vec2(0, 0)));
		m_vertices.push_back(Vertex(p2, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(0, 1)));
		m_vertices.push_back(Vertex(p3, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(1, 1)));
	}

	// neg y
	if (y - 1 >= 0 && !m_blocks.at(getBlockIndex3D(x, y - 1, z)).isSolidBlock())
	{
		m_vertices.push_back(Vertex(p6, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(0, 0)));
		m_vertices.push_back(Vertex(p5, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(1, 0)));
		m_vertices.push_back(Vertex(p2, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(x, y, z), glm::vec2(1, 1)));
		m_vertices.push_back(Vertex(p2, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(x, y, z), glm::vec2(1, 1)));
		m_vertices.push_back(Vertex(p1, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(0, 1)));
		m_vertices.push_back(Vertex(p6, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(0, 0)));
	}

	// pos y
	if (y + 1 < CHUNK_LEN && !m_blocks.at(getBlockIndex3D(x, y + 1, z)).isSolidBlock())
	{
		m_vertices.push_back(Vertex(p7, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(0, 0)));
		m_vertices.push_back(Vertex(p8, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(1, 0)));
		m_vertices.push_back(Vertex(p3, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(x, y, z), glm::vec2(1, 1)));
		m_vertices.push_back(Vertex(p3, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(x, y, z), glm::vec2(1, 1)));
		m_vertices.push_back(Vertex(p4, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(0, 1)));
		m_vertices.push_back(Vertex(p7, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(0, 0)));
	}

	// neg z
	if (z-1 >= 0 && !m_blocks.at(getBlockIndex3D(x, y, z - 1)).isSolidBlock())
	{
		m_vertices.push_back(Vertex(p6, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(0, 0)));
		m_vertices.push_back(Vertex(p5, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(1, 0)));
		m_vertices.push_back(Vertex(p8, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(x, y, z), glm::vec2(1, 1)));
		m_vertices.push_back(Vertex(p8, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(x, y, z), glm::vec2(1, 1)));
		m_vertices.push_back(Vertex(p7, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(0, 1)));
		m_vertices.push_back(Vertex(p6, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(0, 0)));
	}

	// pos z
	if (z+1 < CHUNK_LEN && !m_blocks.at(getBlockIndex3D(x, y, z + 1)).isSolidBlock())
	{
		m_vertices.push_back(Vertex(p1, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(0, 0)));
		m_vertices.push_back(Vertex(p4, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(0, 1)));
		m_vertices.push_back(Vertex(p3, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(x, y, z), glm::vec2(1, 1)));
		m_vertices.push_back(Vertex(p3, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(x, y, z), glm::vec2(1, 1)));
		m_vertices.push_back(Vertex(p2, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(1, 0)));
		m_vertices.push_back(Vertex(p1, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(0, 0)));
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


void Chunk::populateBuffers()
{
	if (m_vertices.size() == 0) // this exists so opengl doesnt freakout if chunk is empty
	{
		return;
	}

	glBindVertexArray(m_VAO);

	m_vb.Bind();
	m_vb.setData(&m_vertices[0], sizeof(m_vertices[0]) * m_vertices.size());

	glEnableVertexAttribArray(POSITION_LOCATION);
	glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(1 * sizeof(glm::vec3)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(2 * sizeof(glm::vec3)));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(glm::vec3)));

	m_vb.Unbind();
}


void Chunk::getBorderBlockStates(Faces direction, bool* blockStates)
{
	// the 'direction' is which direction the data should be pulled from

	switch (direction)
	{
		int x;
		int y;
		int z;

		case Faces::NEGATIVE_X:
			x = 0;

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
		case Faces::POSITIVE_X:
			x = CHUNK_LEN - 1;

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
		case Faces::NEGATIVE_Y:
			y = 0;

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
		case Faces::POSITIVE_Y:
			y = CHUNK_LEN - 1;

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
		case Faces::NEGATIVE_Z:
			z = 0;

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
		case Faces::POSITIVE_Z:
			z = CHUNK_LEN - 1;

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
		default:
			break;
	}
}

bool Chunk::coreBlockHasExposedFaces(int x, int y, int z)
{
	if (x == 0 || x == CHUNK_LEN - 1 || y == 0 || y == CHUNK_LEN - 1 || z == 0 || z == CHUNK_LEN - 1)
		return true;

	return ((!m_blocks.at(getBlockIndex3D(x - 1, y, z)).isSolidBlock()) ||
		(!m_blocks.at(getBlockIndex3D(x + 1, y, z)).isSolidBlock()) ||
		(!m_blocks.at(getBlockIndex3D(x, y - 1, z)).isSolidBlock()) ||
		(!m_blocks.at(getBlockIndex3D(x, y + 1, z)).isSolidBlock()) ||
		(!m_blocks.at(getBlockIndex3D(x, y, z - 1)).isSolidBlock()) ||
		(!m_blocks.at(getBlockIndex3D(x, y, z + 1)).isSolidBlock()));
}

void Chunk::loadFaceNegativeX(int x, int y, int z)
{
	glm::vec3 p1(x - Block::BLOCK_RENDER_SIZE, y - Block::BLOCK_RENDER_SIZE, z + Block::BLOCK_RENDER_SIZE);
	glm::vec3 p2(x + Block::BLOCK_RENDER_SIZE, y - Block::BLOCK_RENDER_SIZE, z + Block::BLOCK_RENDER_SIZE);
	glm::vec3 p3(x + Block::BLOCK_RENDER_SIZE, y + Block::BLOCK_RENDER_SIZE, z + Block::BLOCK_RENDER_SIZE);
	glm::vec3 p4(x - Block::BLOCK_RENDER_SIZE, y + Block::BLOCK_RENDER_SIZE, z + Block::BLOCK_RENDER_SIZE);
	glm::vec3 p5(x + Block::BLOCK_RENDER_SIZE, y - Block::BLOCK_RENDER_SIZE, z - Block::BLOCK_RENDER_SIZE);
	glm::vec3 p6(x - Block::BLOCK_RENDER_SIZE, y - Block::BLOCK_RENDER_SIZE, z - Block::BLOCK_RENDER_SIZE);
	glm::vec3 p7(x - Block::BLOCK_RENDER_SIZE, y + Block::BLOCK_RENDER_SIZE, z - Block::BLOCK_RENDER_SIZE);
	glm::vec3 p8(x + Block::BLOCK_RENDER_SIZE, y + Block::BLOCK_RENDER_SIZE, z - Block::BLOCK_RENDER_SIZE);

	m_vertices.push_back(Vertex(p4, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(1, 1)));
	m_vertices.push_back(Vertex(p1, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(0, 1)));
	m_vertices.push_back(Vertex(p6, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(x, y, z), glm::vec2(0, 0)));
	m_vertices.push_back(Vertex(p6, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(x, y, z), glm::vec2(0, 0)));
	m_vertices.push_back(Vertex(p7, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(1, 0)));
	m_vertices.push_back(Vertex(p4, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(1, 1)));
}

void Chunk::loadFacePositiveX(int x, int y, int z)
{
	glm::vec3 p1(x - Block::BLOCK_RENDER_SIZE, y - Block::BLOCK_RENDER_SIZE, z + Block::BLOCK_RENDER_SIZE);
	glm::vec3 p2(x + Block::BLOCK_RENDER_SIZE, y - Block::BLOCK_RENDER_SIZE, z + Block::BLOCK_RENDER_SIZE);
	glm::vec3 p3(x + Block::BLOCK_RENDER_SIZE, y + Block::BLOCK_RENDER_SIZE, z + Block::BLOCK_RENDER_SIZE);
	glm::vec3 p4(x - Block::BLOCK_RENDER_SIZE, y + Block::BLOCK_RENDER_SIZE, z + Block::BLOCK_RENDER_SIZE);
	glm::vec3 p5(x + Block::BLOCK_RENDER_SIZE, y - Block::BLOCK_RENDER_SIZE, z - Block::BLOCK_RENDER_SIZE);
	glm::vec3 p6(x - Block::BLOCK_RENDER_SIZE, y - Block::BLOCK_RENDER_SIZE, z - Block::BLOCK_RENDER_SIZE);
	glm::vec3 p7(x - Block::BLOCK_RENDER_SIZE, y + Block::BLOCK_RENDER_SIZE, z - Block::BLOCK_RENDER_SIZE);
	glm::vec3 p8(x + Block::BLOCK_RENDER_SIZE, y + Block::BLOCK_RENDER_SIZE, z - Block::BLOCK_RENDER_SIZE);

	m_vertices.push_back(Vertex(p3, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(1, 1)));
	m_vertices.push_back(Vertex(p8, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(1, 0)));
	m_vertices.push_back(Vertex(p5, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(x, y, z), glm::vec2(0, 0)));
	m_vertices.push_back(Vertex(p5, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(x, y, z), glm::vec2(0, 0)));
	m_vertices.push_back(Vertex(p2, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(0, 1)));
	m_vertices.push_back(Vertex(p3, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(1, 1)));

}

void Chunk::loadFaceNegativeY(int x, int y, int z)
{
	glm::vec3 p1(x - Block::BLOCK_RENDER_SIZE, y - Block::BLOCK_RENDER_SIZE, z + Block::BLOCK_RENDER_SIZE);
	glm::vec3 p2(x + Block::BLOCK_RENDER_SIZE, y - Block::BLOCK_RENDER_SIZE, z + Block::BLOCK_RENDER_SIZE);
	glm::vec3 p3(x + Block::BLOCK_RENDER_SIZE, y + Block::BLOCK_RENDER_SIZE, z + Block::BLOCK_RENDER_SIZE);
	glm::vec3 p4(x - Block::BLOCK_RENDER_SIZE, y + Block::BLOCK_RENDER_SIZE, z + Block::BLOCK_RENDER_SIZE);
	glm::vec3 p5(x + Block::BLOCK_RENDER_SIZE, y - Block::BLOCK_RENDER_SIZE, z - Block::BLOCK_RENDER_SIZE);
	glm::vec3 p6(x - Block::BLOCK_RENDER_SIZE, y - Block::BLOCK_RENDER_SIZE, z - Block::BLOCK_RENDER_SIZE);
	glm::vec3 p7(x - Block::BLOCK_RENDER_SIZE, y + Block::BLOCK_RENDER_SIZE, z - Block::BLOCK_RENDER_SIZE);
	glm::vec3 p8(x + Block::BLOCK_RENDER_SIZE, y + Block::BLOCK_RENDER_SIZE, z - Block::BLOCK_RENDER_SIZE);

	m_vertices.push_back(Vertex(p6, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(0, 0)));
	m_vertices.push_back(Vertex(p5, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(1, 0)));
	m_vertices.push_back(Vertex(p2, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(x, y, z), glm::vec2(1, 1)));
	m_vertices.push_back(Vertex(p2, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(x, y, z), glm::vec2(1, 1)));
	m_vertices.push_back(Vertex(p1, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(0, 1)));
	m_vertices.push_back(Vertex(p6, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(0, 0)));
}

void Chunk::loadFacePositiveY(int x, int y, int z)
{
	glm::vec3 p1(x - Block::BLOCK_RENDER_SIZE, y - Block::BLOCK_RENDER_SIZE, z + Block::BLOCK_RENDER_SIZE);
	glm::vec3 p2(x + Block::BLOCK_RENDER_SIZE, y - Block::BLOCK_RENDER_SIZE, z + Block::BLOCK_RENDER_SIZE);
	glm::vec3 p3(x + Block::BLOCK_RENDER_SIZE, y + Block::BLOCK_RENDER_SIZE, z + Block::BLOCK_RENDER_SIZE);
	glm::vec3 p4(x - Block::BLOCK_RENDER_SIZE, y + Block::BLOCK_RENDER_SIZE, z + Block::BLOCK_RENDER_SIZE);
	glm::vec3 p5(x + Block::BLOCK_RENDER_SIZE, y - Block::BLOCK_RENDER_SIZE, z - Block::BLOCK_RENDER_SIZE);
	glm::vec3 p6(x - Block::BLOCK_RENDER_SIZE, y - Block::BLOCK_RENDER_SIZE, z - Block::BLOCK_RENDER_SIZE);
	glm::vec3 p7(x - Block::BLOCK_RENDER_SIZE, y + Block::BLOCK_RENDER_SIZE, z - Block::BLOCK_RENDER_SIZE);
	glm::vec3 p8(x + Block::BLOCK_RENDER_SIZE, y + Block::BLOCK_RENDER_SIZE, z - Block::BLOCK_RENDER_SIZE);

	m_vertices.push_back(Vertex(p7, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(0, 0)));
	m_vertices.push_back(Vertex(p8, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(1, 0)));
	m_vertices.push_back(Vertex(p3, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(x, y, z), glm::vec2(1, 1)));
	m_vertices.push_back(Vertex(p3, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(x, y, z), glm::vec2(1, 1)));
	m_vertices.push_back(Vertex(p4, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(0, 1)));
	m_vertices.push_back(Vertex(p7, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(0, 0)));

}

void Chunk::loadFaceNegativeZ(int x, int y, int z)
{
	glm::vec3 p1(x - Block::BLOCK_RENDER_SIZE, y - Block::BLOCK_RENDER_SIZE, z + Block::BLOCK_RENDER_SIZE);
	glm::vec3 p2(x + Block::BLOCK_RENDER_SIZE, y - Block::BLOCK_RENDER_SIZE, z + Block::BLOCK_RENDER_SIZE);
	glm::vec3 p3(x + Block::BLOCK_RENDER_SIZE, y + Block::BLOCK_RENDER_SIZE, z + Block::BLOCK_RENDER_SIZE);
	glm::vec3 p4(x - Block::BLOCK_RENDER_SIZE, y + Block::BLOCK_RENDER_SIZE, z + Block::BLOCK_RENDER_SIZE);
	glm::vec3 p5(x + Block::BLOCK_RENDER_SIZE, y - Block::BLOCK_RENDER_SIZE, z - Block::BLOCK_RENDER_SIZE);
	glm::vec3 p6(x - Block::BLOCK_RENDER_SIZE, y - Block::BLOCK_RENDER_SIZE, z - Block::BLOCK_RENDER_SIZE);
	glm::vec3 p7(x - Block::BLOCK_RENDER_SIZE, y + Block::BLOCK_RENDER_SIZE, z - Block::BLOCK_RENDER_SIZE);
	glm::vec3 p8(x + Block::BLOCK_RENDER_SIZE, y + Block::BLOCK_RENDER_SIZE, z - Block::BLOCK_RENDER_SIZE);

	m_vertices.push_back(Vertex(p6, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(0, 0)));
	m_vertices.push_back(Vertex(p5, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(1, 0)));
	m_vertices.push_back(Vertex(p8, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(x, y, z), glm::vec2(1, 1)));
	m_vertices.push_back(Vertex(p8, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(x, y, z), glm::vec2(1, 1)));
	m_vertices.push_back(Vertex(p7, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(0, 1)));
	m_vertices.push_back(Vertex(p6, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(0, 0)));
}

void Chunk::loadFacePositiveZ(int x, int y, int z)
{
	glm::vec3 p1(x - Block::BLOCK_RENDER_SIZE, y - Block::BLOCK_RENDER_SIZE, z + Block::BLOCK_RENDER_SIZE);
	glm::vec3 p2(x + Block::BLOCK_RENDER_SIZE, y - Block::BLOCK_RENDER_SIZE, z + Block::BLOCK_RENDER_SIZE);
	glm::vec3 p3(x + Block::BLOCK_RENDER_SIZE, y + Block::BLOCK_RENDER_SIZE, z + Block::BLOCK_RENDER_SIZE);
	glm::vec3 p4(x - Block::BLOCK_RENDER_SIZE, y + Block::BLOCK_RENDER_SIZE, z + Block::BLOCK_RENDER_SIZE);
	glm::vec3 p5(x + Block::BLOCK_RENDER_SIZE, y - Block::BLOCK_RENDER_SIZE, z - Block::BLOCK_RENDER_SIZE);
	glm::vec3 p6(x - Block::BLOCK_RENDER_SIZE, y - Block::BLOCK_RENDER_SIZE, z - Block::BLOCK_RENDER_SIZE);
	glm::vec3 p7(x - Block::BLOCK_RENDER_SIZE, y + Block::BLOCK_RENDER_SIZE, z - Block::BLOCK_RENDER_SIZE);
	glm::vec3 p8(x + Block::BLOCK_RENDER_SIZE, y + Block::BLOCK_RENDER_SIZE, z - Block::BLOCK_RENDER_SIZE);

	m_vertices.push_back(Vertex(p1, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(0, 0)));
	m_vertices.push_back(Vertex(p4, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(0, 1)));
	m_vertices.push_back(Vertex(p3, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(x, y, z), glm::vec2(1, 1)));
	m_vertices.push_back(Vertex(p3, glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(x, y, z), glm::vec2(1, 1)));
	m_vertices.push_back(Vertex(p2, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(1, 0)));
	m_vertices.push_back(Vertex(p1, glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(x, y, z), glm::vec2(0, 0)));
}
