#include "World.h"

#include "Chunk.h"

#include <unordered_map>
#include <memory>




namespace world
{
	void generateWorld();
	bool createChunk(chunkOffset pos);
	void bakeChunk(chunkOffset chunkPos);
	std::string getChunkKeyFromPos(chunkOffset pos);
	bool keyExists(chunkOffset pos);

	std::unordered_map<std::string, std::shared_ptr<Chunk>> chunkMap;
	int renderDistance = 6;

	bool initWorld()
	{
		generateWorld();
		return false;
	}

	void renderWorld(ShaderProgram program)
	{
		for (auto i = chunkMap.begin(); i != chunkMap.end(); i++)
		{
			i->second->render(program);
		}
	}



	void generateWorld()
	{
		for (int x = -renderDistance / 2; x < renderDistance / 2; x++)
		{
			uint32_t y = 1;
			//for (int y = -worldSize / 2; y < worldSize / 2; y++)
			{
				for (int z = -renderDistance / 2; z < renderDistance / 2; z++)
				{
					glm::vec3 f = g_camera->position;


					//createChunk(CHUNK_OFFSET(f.x + x, f.y + y, f.z + z));
					createChunk(chunkOffset(f.x + x, 0, f.z + z));
				}
			}
		}
	}

	void bakeWorld()
	{
		for (auto i = chunkMap.begin(); i != chunkMap.end(); i++)
		{
			bakeChunk(i->second->getChunkOffset());
		}
	}


	bool createChunk(chunkOffset pos)
	{
		if (keyExists(pos))
			return false;

		std::string key = getChunkKeyFromPos(pos);

		chunkMap.insert({ key, std::make_shared<Chunk>(pos) });

		return true;
	}

	// entry point for any chunk baking. direct chunk baking is unadvisable
	void bakeChunk(chunkOffset chunkPos)
	{
		std::vector<std::shared_ptr<Chunk>> neighbourChunks;

		if (keyExists(chunkPos + chunkOffset(-1,  0,  0))) neighbourChunks.emplace_back(chunkMap.at(getChunkKeyFromPos(chunkPos + chunkOffset(-1,  0,  0))));
		if (keyExists(chunkPos + chunkOffset( 1,  0,  0))) neighbourChunks.emplace_back(chunkMap.at(getChunkKeyFromPos(chunkPos + chunkOffset( 1,  0,  0))));
		if (keyExists(chunkPos + chunkOffset( 0, -1,  0))) neighbourChunks.emplace_back(chunkMap.at(getChunkKeyFromPos(chunkPos + chunkOffset( 0, -1,  0))));
		if (keyExists(chunkPos + chunkOffset( 0,  1,  0))) neighbourChunks.emplace_back(chunkMap.at(getChunkKeyFromPos(chunkPos + chunkOffset( 0,  1,  0))));
		if (keyExists(chunkPos + chunkOffset( 0,  0, -1))) neighbourChunks.emplace_back(chunkMap.at(getChunkKeyFromPos(chunkPos + chunkOffset( 0,  0, -1))));
		if (keyExists(chunkPos + chunkOffset( 0,  0,  1))) neighbourChunks.emplace_back(chunkMap.at(getChunkKeyFromPos(chunkPos + chunkOffset( 0,  0,  1))));


		chunkMap.at(getChunkKeyFromPos(chunkPos))->clearData();
		chunkMap.at(getChunkKeyFromPos(chunkPos))->bake(neighbourChunks);
		chunkMap.at(getChunkKeyFromPos(chunkPos))->populateBuffers();




		//std::shared_ptr<Chunk> workingChunk = chunkMap.at(getChunkKeyFromPos(chunkPos));


		//workingChunk->clearData();
		//workingChunk->bakeCore();

		//for (int i = 0; i < Chunk::Faces::NUM_FACES; i++)
		//{
		//	// get direction
		//	glm::ivec3 axis = { 0, 0, 0 };
		//	switch (i)
		//	{
		//	case Chunk::Faces::NEGATIVE_X:
		//		axis.x--;
		//		break;
		//	case Chunk::Faces::POSITIVE_X:
		//		axis.x++;
		//		break;
		//	case Chunk::Faces::NEGATIVE_Y:
		//		axis.y--;
		//		break;
		//	case Chunk::Faces::POSITIVE_Y:
		//		axis.y++;
		//		break;
		//	case Chunk::Faces::NEGATIVE_Z:
		//		axis.z--;
		//		break;
		//	case Chunk::Faces::POSITIVE_Z:
		//		axis.z++;
		//		break;
		//	default:
		//		break;
		//	}

		//	// eject if chunk nonexistant
		//	if (!keyExists(chunkPos + axis))
		//		continue;

		//	std::shared_ptr<Chunk> secondaryChunk = chunkMap.at(getChunkKeyFromPos(chunkPos + axis));

		//	int secondaryFace;
		//	if ((Chunk::Faces)i == Chunk::Faces::NEGATIVE_X || (Chunk::Faces)i == Chunk::Faces::NEGATIVE_Y || (Chunk::Faces)i == Chunk::Faces::NEGATIVE_Z)
		//		secondaryFace = i + 1;
		//	else
		//		secondaryFace = i - 1;

		//	bool secondaryChunkBlockStates[Chunk::CHUNK_AREA];
		//	secondaryChunk->getBorderBlockStates((Chunk::Faces)secondaryFace, secondaryChunkBlockStates);

		//	workingChunk->bakeBorder((Chunk::Faces)i, secondaryChunkBlockStates);
		//}

		//workingChunk->populateBuffers();
	}


	std::string getChunkKeyFromPos(chunkOffset pos)
	{
		std::string xNum = std::to_string(pos.x);
		std::string yNum = std::to_string(pos.y);
		std::string zNum = std::to_string(pos.z);

		return (xNum + "X" + yNum + "Y" + zNum + "Z");
	}

	bool keyExists(chunkOffset pos)
	{
		std::string key = getChunkKeyFromPos(pos);

		if (chunkMap.find(key) == chunkMap.end())
			return false;
		else
			return true;
	}
}