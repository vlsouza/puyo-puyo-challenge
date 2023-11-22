#ifndef BOARD_H
#define BOARD_H

#include <cstring>
#include "types.h"
#include "block.h"

#define GRID_SIZE 37

inline void
matrix_set(u8 *values, s32 width, s32 row, s32 col, u8 value)
{
	s32 index = row * width + col;
	values[index] = value;
}

inline u8
matrix_get(const u8 *values, s32 width, s32 row, s32 col)
{
	s32 index = row * width + col;
	return values[index];
}

inline u8
check_row_filled(const u8 *values, s32 width, s32 row)
{
	for (s32 col = 0; col < width; ++col) 
	{
		if (!matrix_get(values, width, row, col)) // if cell is empty
		{
			return 0;
		}
	}

	return 1; // filled row if none of the row squares are empty
}

inline u8
check_row_empty(const u8 *values, s32 width, s32 row)
{
	for (s32 col = 0; col < width; ++col) 
	{
		if (matrix_get(values, width, row, col)) // if cell is empty
		{
			return 0;
		}
	}

	return 1;
}

inline s32 
find_lines(const u8 *values, s32 width, s32 height, u8 *lines_out)
{
	s32 count = 0;
	for (s32 row = 0; row < height; ++row) 
	{
		u8 filled = check_row_filled(values, width, row);
		lines_out[row] = filled;
		count += filled;
	}

	return count; //lines filled
}

inline void 
clear_lines(u8 *values, s32 width, s32 height, u8 *lines)
{
	s32 src_row = height - 1; //coursor
	for(s32 dst_row = height - 1; dst_row >= 0; --dst_row)
	{
		while (src_row > 0 && lines[src_row])
		{
			--src_row;
		}

		if (src_row < 0)
		{
			memset(values + dst_row * width, 0, width);
		}
		else
		{
			if (src_row != dst_row)
            {
				memcpy(values + dst_row * width, values + src_row * width, width);
			}
			--src_row;
		}	
	}
}

// Adicione esta função para verificar se uma posição no tabuleiro está vazia
inline bool 
is_empty(const u8 *board, s32 width, s32 row, s32 col) {
    return matrix_get(board, width, row, col) == 0;
}

// render board functions
inline void 
fill_rect(SDL_Renderer * renderer, s32 x, s32 y, s32 width, s32 height, Color color) // fill rectangle
{
	SDL_Rect rect = {};
	rect.x = x;
	rect.y = y;
	rect.w = width;
	rect.h = height;
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	SDL_RenderFillRect(renderer, &rect);
}

inline void 
draw_rect(SDL_Renderer *renderer,
          s32 x, s32 y, s32 width, s32 height, Color color)
{
    SDL_Rect rect = {};
    rect.x = x;
    rect.y = y;
    rect.w = width;
    rect.h = height;
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawRect(renderer, &rect);
}

inline void 
draw_cell(SDL_Renderer *renderer,
          s32 row, s32 col, u8 value,
          s32 offset_x, s32 offset_y,
          bool outline = false)
{
    Color base_color = BASE_COLORS[value];
    Color light_color = LIGHT_COLORS[value];
    Color dark_color = DARK_COLORS[value];

    s32 edge = GRID_SIZE / 8;

    s32 x = col * GRID_SIZE + offset_x;
    s32 y = row * GRID_SIZE + offset_y;

    if (outline)
    {
        draw_rect(renderer, x, y, GRID_SIZE, GRID_SIZE, base_color);
        return;
    }
    
    fill_rect(renderer, x, y, GRID_SIZE, GRID_SIZE, dark_color); // set dark background color
    fill_rect(renderer, x + edge, y,
              GRID_SIZE - edge, GRID_SIZE - edge, light_color);
	fill_rect(renderer, x + edge, y + edge, GRID_SIZE - edge * 2 /* margin */, GRID_SIZE - edge * 2 /* margin */, base_color);
}

inline void 
draw_board(SDL_Renderer *renderer, const u8 *board, s32 width, s32 height, s32 offset_x, s32 offset_y)
{
	// loops trought all of the cells
	for(s32 row = 0; row < height; ++row)
	{
		for(s32 col = 0; col < width; ++col)
		{
			u8 value = matrix_get(board, width, row, col);
			draw_cell(renderer, row, col, value, offset_x, offset_y);
			//if(value)
			//{
			//	draw_cell(renderer, row, col, value, offset_x, offset_y);
			//}
		}
	}
}

#endif // BOARD_H