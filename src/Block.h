#ifndef BLOCK_H
#define BLOCK_H

#include "types.h"

// two blocks together represents one unit of piece
struct Block
{
    const u8 *data;
    const s32 side; // dimensions
};

inline Block
block(const u8 *data, s32 side)
{
	return { data, side };
}

inline u8
block_get(const Block *block, s32 row, s32 col, s32 rotation)
{
	s32 side = block->side;

	switch (rotation)
	{
	case 0:
		return block->data[row * side + col];
	case 1:
		return block->data[(side - col - 1) * side + row];
	case 2:
		return block->data[(side - row - 1) * side + (side - col - 1)];
	case 3:
		return block->data[col * side + (side - row - 1)];
	}

	return 0;
}

const u8 BLOCK_1[] = {
	0,1,0,
	0,0,0,
	0,0,0
};

const u8 BLOCK_2[] = {
	0,2,0,
	0,0,0,
	0,0,0
};

const u8 BLOCK_3[] = {
	0,3,0,
	0,0,0,
	0,0,0
};

const u8 BLOCK_4[] = {
	0,4,0,
	0,0,0,
	0,0,0
};

const Block BLOCKS[] = {
	block(BLOCK_1, 3),
	block(BLOCK_2, 3),
	block(BLOCK_3, 3),
	block(BLOCK_4, 3),
};

#endif // BLOCK_H
