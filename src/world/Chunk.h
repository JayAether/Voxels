#pragma once


#include "../engine/ShaderProgram.h"
#include "blockData.h"

#include <bitset>
#include <unordered_map>




typedef glm::ivec3	chunkOffset;
typedef uint32_t	blockKey;


typename int32_t shaderVertexPosition	= 0;
typename int32_t shaderVertexNormal		= 1;
typename int32_t shaderVertexTexCoord	= 2;



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
	static const int32_t CHUNK_LEN = 32;
	static const int32_t CHUNK_AREA = CHUNK_LEN * CHUNK_LEN;
	static const int32_t CHUNK_VOL = CHUNK_LEN * CHUNK_LEN * CHUNK_LEN;

	Chunk(chunkOffset chunkPos);
	~Chunk();

	void render(ShaderProgram program);


	void generateMap();

	void bake(std::vector<std::shared_ptr<Chunk>> neighbourChunks);

	bool blockStateActive(glm::ivec3 relativeBlockPos);

	void bakeCore();
	void bakeBorder(Faces direction, bool* blockStates);

	bool coreBlockHasExposedFaces(int x, int y, int z) const;

	void getBorderBlockStates(Faces direction, bool* blockStates) const;
	
	void clearData();
	void populateBuffers();

	chunkOffset getChunkOffset() const;

private:
	inline static const double BLOCK_RENDER_SIZE = 1.0;

	int getBlockIndex3D(int x, int y, int z) const;
	int getBlockIndex2D(int x, int y) const;

	//void setBlock(BlockID blockType, int x, int y, int z);

	void createCoreBlock(int x, int y, int z);
	void createBorderBlock(Faces direction, int x, int y, int z);

	void loadFaceNegativeX(int x, int y, int z);
	void loadFacePositiveX(int x, int y, int z);
	void loadFaceNegativeY(int x, int y, int z);
	void loadFacePositiveY(int x, int y, int z);
	void loadFaceNegativeZ(int x, int y, int z);
	void loadFacePositiveZ(int x, int y, int z);



private:
	// chunk offset works its own coordinate space aside from the true world coords
	// chunkOffset times chunkSize plus local coords within the chunk equals true world coords
	chunkOffset m_chunkOffset;

	std::vector<std::vector<std::bitset<CHUNK_LEN>>> m_active;
	std::unordered_map<blockKey, world::block_data> m_blockData;

	std::vector<Vertex>		m_vertData;
	//std::vector<glm::vec3>	m_vertPos;
	//std::vector<glm::vec3>	m_vertNorm;
	//std::vector<glm::vec2>	m_vertTC;
	//std::vector<uint32_t>	m_vertIndices;
	
	unsigned int m_VAO;
	//uint32_t m_vb[NUM_BUFFERS];
	uint32_t m_vb;





	//FastNoiseLite noise;
};

