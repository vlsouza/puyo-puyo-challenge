#ifndef BLOCK_H
#define BLOCK_H

#include <cassert>
#include "types.h"
#include "board.h"

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


struct Block_State
{
	u8 block_index; 

	s32 offset_row; // where the piece current is
	s32 offset_col;

	s32 rotation;

	bool merged;
};

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

inline bool 
check_block_valid(const Block_State *block_state, const u8 *board, s32 width, s32 height)
{
	const Block *block = BLOCKS + block_state->block_index;
	assert(block);

	// check all piece cells
	for (s32 row = 0; row < block->side; ++row) 
	{
		for (s32 col = 0; col < block->side; ++col)
		{
			u8 value = block_get(block, row, col, block_state->rotation);
			if(value > 0)
			{
				s32 board_row = block_state->offset_row + row;
				s32 board_col = block_state->offset_col + col;
				// collision detection in the grid
				if (board_row < 0)
				{
					return false;
				}
				if (board_row >= height)
				{
					return false;
				}
				if (board_col < 0)
				{
					return false;
				}
				if (board_col >= width)
				{
					return false;
				}
				if (matrix_get(board, width, board_row, board_col))
				{
					return false;
				}
			}
		}
	}

	return true;
}


#endif // BLOCK_H

