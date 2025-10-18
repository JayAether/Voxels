#pragma once


#include "../engine/ShaderProgram.h"
#include "voxels/Block.h"

#include <bitset>
#include <memory>

#define CHUNK_OFFSET glm::ivec3 

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

#define POSITION_LOCATION  0
#define TEX_COORD_LOCATION 1
#define NORMAL_LOCATION    2



struct Vertex
{
	Vertex(glm::vec3 pos, glm::vec3 norm, glm::vec2 texCoord)
		: pos(pos), norm(norm), texCoord(texCoord)
	{

	}

	glm::vec3 pos;
	glm::vec3 norm;
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

public:

	Chunk(CHUNK_OFFSET chunkPos);
	~Chunk();

	void render(ShaderProgram program);


	void generateMap();

	void bakeCore();
	void bakeBorder(Faces direction, bool* blockStates);

	bool coreBlockHasExposedFaces(int x, int y, int z) const;

	void getBorderBlockStates(Faces direction, bool* blockStates) const;
	
	void clearData();
	void populateBuffers();

	CHUNK_OFFSET getChunkOffset() const;


private:
	int getBlockIndex3D(int x, int y, int z) const;
	int getBlockIndex2D(int x, int y) const;

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
	static const int CHUNK_LEN = 32;
	static const int CHUNK_AREA = CHUNK_LEN * CHUNK_LEN;
	static const int CHUNK_VOL = CHUNK_LEN * CHUNK_LEN * CHUNK_LEN;
private:
	// the offset off of world's 0,0,0. ie multiply this by chunk size to get the chunk position
	CHUNK_OFFSET m_chunkOffset;

	std::vector<std::vector<std::bitset<CHUNK_LEN>>> m_active;

	unsigned int m_VAO;
	//uint32_t m_vb[NUM_BUFFERS];
	uint32_t m_vb;

	std::vector<Vertex> m_data;


	//FastNoiseLite noise;
};

