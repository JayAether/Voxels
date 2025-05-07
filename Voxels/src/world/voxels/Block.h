#pragma once

#include "BlockID.h"


class Block
{
public:
	Block()
	{
		setAir_block();
		//setStone_block();
	}
	

	Block(BlockID blockType) 
	{
		switch (blockType)
		{
		case DEFAULT_AIR_BLOCK:
			setAir_block();
			break;
		case COLORED_BLOCK:
			setColored_block();
			break;
		case GRASS_BLOCK:
			setGrass_block();
			break;
		case DIRT_BLOCK:
			setDirt_block();
			break;
		case STONE_BLOCK:
			setStone_block();
			break;
		default:
			break;
		}
	}

	void changeType(BlockID blockType)
	{
		switch (blockType)
		{
		case DEFAULT_AIR_BLOCK:
			setAir_block();
			break;
		case COLORED_BLOCK:
			setColored_block();
			break;
		case GRASS_BLOCK:
			setGrass_block();
			break;
		case DIRT_BLOCK:
			setDirt_block();
			break;
		case STONE_BLOCK:
			setStone_block();
			break;
		default:
			break;
		}
	}

	void setActive(bool status);

	bool isActive();
	bool isTransparent() const;
	bool isLiquid() const;

	bool isSolidBlock() const;

	BlockID getBlockID();

	void setAir_block();
	void setColored_block();
	void setGrass_block();
	void setDirt_block();
	void setStone_block();

	static const int BLOCK_RENDER_SIZE = 1;
protected:
	BlockID m_blockType;
	
	bool m_active;

	bool m_transparent;
	bool m_liquid;


private:
};

