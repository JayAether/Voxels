#pragma once


#include "Chunk.h"

#include <unordered_map>


class World
{
public:


	
	void render(ShaderProgram program)
	{
		for (auto i = m_allChunks.begin(); i != m_allChunks.end(); i++)
		{
			i->second->render(program);
		}
	}


	void generateWorld()
	{
		for (int x = 0; x < worldSize; x++)
		{
			for (int z = 0; z < worldSize; z++)
			{
				createChunk(CHUNK_POS(x, 0, z));
			}
		}
	}

	void bakeWorld()
	{
		for (auto i = m_allChunks.begin(); i != m_allChunks.end(); i++)
		{
			bakeChunk(i->second->m_chunkPos);
		}
	}


	bool createChunk(CHUNK_POS pos)
	{
		if (keyExists(pos))
			return false;

		Chunk *chunk = new Chunk(pos);
		std::string key = getChunkKeyFromPos(pos);

		m_allChunks.insert({ key, chunk });

		return true;
	}

	// entry point for any chunk baking. direct chunk baking is unadvisable
	void bakeChunk(CHUNK_POS chunkPos)
	{
		Chunk* workingChunk = m_allChunks.at(getChunkKeyFromPos(chunkPos));

		workingChunk->clearBufferData();
		workingChunk->bakeCore();

		for (int i = 0; i < Faces::NUM_FACES; i++)
		{
			// get direction
			glm::ivec3 axis = { 0, 0, 0 };
			switch (i)
			{
			case Faces::NEGATIVE_X:
				axis.x--;
				break;
			case Faces::POSITIVE_X:
				axis.x++;
				break;
			case Faces::NEGATIVE_Y:
				axis.y--;
				break;
			case Faces::POSITIVE_Y:
				axis.y++;
				break;
			case Faces::NEGATIVE_Z:
				axis.z--;
				break;
			case Faces::POSITIVE_Z:
				axis.z++;
				break;
			default:
				break;
			}

			// eject if chunk nonexistant
			if (!keyExists(chunkPos + axis))
				continue;

			Chunk* secondaryChunk = m_allChunks.at(getChunkKeyFromPos(chunkPos + axis));

			int secondaryFace;
			if ((Faces)i == Faces::NEGATIVE_X || (Faces)i == Faces::NEGATIVE_Y || (Faces)i == Faces::NEGATIVE_Z)
				secondaryFace = i +1;
			else
				secondaryFace = i -1;

			bool secondaryChunkBlockStates[Chunk::CHUNK_AREA];
			secondaryChunk->getBorderBlockStates((Faces)secondaryFace, secondaryChunkBlockStates);

			workingChunk->bakeBorder((Faces)i, secondaryChunkBlockStates);
		}

		workingChunk->populateBuffers();
	}

	std::string getChunkKeyFromPos(CHUNK_POS pos)
	{
		std::string xNum = std::to_string(pos.x);
		std::string yNum = std::to_string(pos.y);
		std::string zNum = std::to_string(pos.z);
		
		return (xNum + "X" + yNum + "Y" + zNum + "Z");
	}

	bool keyExists(CHUNK_POS pos)
	{
		std::string key = getChunkKeyFromPos(pos);

		if (m_allChunks.find(key) == m_allChunks.end())
			return false;
		else
			return true;
	}

	CHUNK_POS getChunkPosFromKey(std::string)
	{
		// this should return an glm::ivec3
	}

private:
	std::unordered_map<std::string, Chunk*> m_allChunks;

	int worldSize = 12;
};



