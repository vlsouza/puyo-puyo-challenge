#ifndef PIECE_H
#define PIECE_H

#include "block.h"
#include "board.h"
#include "game.h"

inline void
draw_piece(SDL_Renderer *renderer,
           const Block_State *piece,
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