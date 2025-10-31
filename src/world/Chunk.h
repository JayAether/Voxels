#pragma once


#include "../engine/ShaderProgram.h"
#include "blockData.h"

#include <bitset>
#include <unordered_map>
#include <memory>


typedef glm::ivec3		chunkOffset;
typedef std::bitset<24>	blockKey;



class Chunk
{
	struct vertex
	{
		glm::vec3 pos;
		glm::vec3 norm;
		glm::vec2 tc;

		vertex(glm::vec3 pos, glm::vec3 norm, glm::vec2 texCoord) : pos(pos), norm(norm), tc(texCoord) {}
	};

public:
	static const uint8_t CHUNK_LEN = 32; // this is expected to be an 8 bit int due to the block keys designed around that
	static const uint32_t CHUNK_AREA = (uint32_t)CHUNK_LEN * CHUNK_LEN;
	static const uint32_t CHUNK_VOL = CHUNK_LEN * CHUNK_LEN * CHUNK_LEN;

	Chunk(chunkOffset chunkPos);
	~Chunk();

	void render(ShaderProgram program);

	void generateMap();

	void bake(std::vector<std::shared_ptr<Chunk>> neighbourChunks);

	bool blockStateActive(glm::ivec3 relativeBlockPos);
	
	void clearData();
	void populateBuffers();

	chunkOffset getChunkOffset() const;

private:
	inline static const double BLOCK_RENDER_SIZE = 1.0;

	// chunk offset works its own coordinate space aside from the true world coords
	// chunkOffset times chunkSize plus local coords within the chunk equals true world coords
	chunkOffset m_chunkOffset;

	std::vector<std::vector<std::bitset<CHUNK_LEN>>>	m_active;		// active blocks
	std::unordered_map<blockKey, world::block_data>		m_blockData;	// block types

	std::vector<vertex>		m_vertData;
	std::vector<uint32_t>	m_vertIndices;

	uint32_t m_VAO;
	uint32_t m_vb;
	uint32_t m_ib;


	int getBlockIndex3D(int x, int y, int z) const;
	int getBlockIndex2D(int x, int y) const;

	blockKey getBlockKey(uint32_t x, uint32_t y, uint32_t z);

	void loadFaceNegativeX(int x, int y, int z);
	void loadFacePositiveX(int x, int y, int z);
	void loadFaceNegativeY(int x, int y, int z);
	void loadFacePositiveY(int x, int y, int z);
	void loadFaceNegativeZ(int x, int y, int z);
	void loadFacePositiveZ(int x, int y, int z);
};

