#pragma once


#include "../engine/ShaderProgram.h"
#include "voxels/Block.h"
#include "../engine/graphics/VertexBuffer.h"

#include <memory>

#define CHUNK_POS glm::ivec3 

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

#define POSITION_LOCATION  0
#define TEX_COORD_LOCATION 1
#define NORMAL_LOCATION    2

enum Faces 
{
	NEGATIVE_X,
	POSITIVE_X,
	NEGATIVE_Y,
	POSITIVE_Y,
	NEGATIVE_Z,
	POSITIVE_Z,
	NUM_FACES,
};

struct Vertex
{
	Vertex(glm::vec3 pos, glm::vec3 color, glm::vec3 offset, glm::vec2 texCoord)
		: pos(pos), color(color), offset(offset), texCoord(texCoord)
	{

	}

	glm::vec3 pos;
	glm::vec3 color;
	glm::vec3 offset;
	glm::vec2 texCoord;
};


class Chunk
{
	enum Buffers
	{
		POS_BUFFER,
		COLOR_BUFFER,
		OFFSET_BUFFER,
		NUM_BUFFERS,
	};

public:

	Chunk(CHUNK_POS chunkPos);

	~Chunk();

	void update(float dt);
	void render(ShaderProgram program);

	void reserveBlockData();

	void generateMap();

	void bakeCore();
	void bakeBorder(Faces direction, bool* blockStates);

	// this assumes that this is a core block
	bool coreBlockHasExposedFaces(int x, int y, int z);

	void getBorderBlockStates(Faces direction, bool* blockStates);
	
	void clearBufferData();
	void populateBuffers();



private:
	int getBlockIndex3D(int x, int y, int z);
	int getBlockIndex2D(int x, int y);

	void setBlock(BlockID blockType, int x, int y, int z);

	void createCoreBlock(int x, int y, int z);
	void createBorderBlock(Faces direction, int x, int y, int z);

	void loadFaceNegativeX(int x, int y, int z);
	void loadFacePositiveX(int x, int y, int z);
	void loadFaceNegativeY(int x, int y, int z);
	void loadFacePositiveY(int x, int y, int z);
	void loadFaceNegativeZ(int x, int y, int z);
	void loadFacePositiveZ(int x, int y, int z);

public:

	// the offset off of world's 0,0,0. ie multiply this by chunk size to get the chunk position
	CHUNK_POS m_chunkPos;


	static const int CHUNK_LEN = 32;
	static const int CHUNK_AREA = CHUNK_LEN * CHUNK_LEN;
	static const int CHUNK_VOL = CHUNK_LEN * CHUNK_LEN * CHUNK_LEN;

private:

	std::vector<Block> m_blocks;

	unsigned int m_VAO;
	unsigned int m_buffer;
	VertexBuffer m_vb;

	std::vector<Vertex> m_vertices;


	//FastNoiseLite noise;
};

