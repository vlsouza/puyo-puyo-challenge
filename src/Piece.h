#ifndef PIECE_H
#define PIECE_H

#include <cassert>
#include "block.h"
#include "board.h"
#include "game.h"

struct Piece_State
{
	u8 block_index; 

	s32 offset_row; // where the piece currently is
	s32 offset_col;

	s32 rotation;

	bool merged;
};

inline bool 
check_piece_valid(const Piece_State *piece, const u8 *board, s32 width, s32 height)
{
	const Block *block = BLOCKS + piece->block_index;
	assert(block);

	// check all piece cells
	for (s32 row = 0; row < block->side; ++row) 
	{
		for (s32 col = 0; col < block->side; ++col)
		{
			u8 value = block_get(block, row, col, piece->rotation);
			if(value > 0)
			{
				s32 board_row = piece->offset_row + row;
				s32 board_col = piece->offset_col + col;
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

inline void
draw_piece(SDL_Renderer *renderer,
           const Piece_State *piece,
           s32 offset_x, s32 offset_y,
           bool outline = false)
{
    const Block *block = BLOCKS + piece->block_index;
    for (s32 row = 0;
         row < block->side;
         ++row)
    {
        for (s32 col = 0;
             col < block->side;
             ++col)
        {
            u8 value = block_get(block, row, col, piece->rotation);
            if (value)
            {
                draw_cell(renderer,
                          row + piece->offset_row,
                          col + piece->offset_col,
                          value,
                          offset_x, offset_y,
                          outline);
            }
        }
    }
}

#endif // PIECE_H