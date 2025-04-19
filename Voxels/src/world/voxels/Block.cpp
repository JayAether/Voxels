#include "Block.h"

void Block::setActive(bool status)
{
	m_active = status;
}

bool Block::isActive()
{
	return m_active;
}

bool Block::isTransparent()
{
	return m_transparent;
}

bool Block::isLiquid()
{
	return m_liquid;
}

bool Block::isSolidBlock()
{

	if (
		isTransparent() ||
		isLiquid())
	{
		return false;
	}
	else
		return true;

}


BlockID Block::getBlockID()
{	
	return m_blockType;	
}




void Block::setAir_block()
{
	m_blockType = DEFAULT_AIR_BLOCK;
	m_active = false;
	m_transparent = true;
	m_liquid = false;
}

void Block::setColored_block()
{
	m_blockType = COLORED_BLOCK;
	m_active = true;
	m_transparent = false;
	m_liquid = false;
}

void Block::setGrass_block()
{
	m_blockType = GRASS_BLOCK;
	m_active = true;
	m_transparent = false;
	m_liquid = false;
}

void Block::setDirt_block()
{
	m_blockType = DIRT_BLOCK;
	m_active = true;
	m_transparent = false;
	m_liquid = false;
}

void Block::setStone_block()
{
	m_blockType = STONE_BLOCK;
	m_active = true;
	m_transparent = false;
	m_liquid = false;
}

