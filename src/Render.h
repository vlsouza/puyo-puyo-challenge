#include <cstdlib>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <cassert>

#include <SDL2/SDL.h> // platform layer
#include <SDL2/SDL_ttf.h>

#include "game.h"
#include "types.h"
#include "color.h"
#include "block.h"
#include "piece.h"
#include "board.h"
#include "label.h"
#include "level.h"

void update_game_start(Game_State *game, const Input_State *input)
{
	if(input->dup > 0)
	{
		++game->start_level;
	}
	if(input->ddown > 0 && game->start_level > 0)
	{
		--game->start_level;
	}
	if(input->da > 0)
	{
		// restart level after game over
		memset(game->board, 0, WIDTH *HEIGHT);
		game->level = game->start_level;
		game->line_count = 0;
		game->points = 0;
		spawn_piece(game);
		game->phase = GAME_PHASE_PLAY;
	}
}

void update_game_gameover(Game_State *game, const Input_State *input)
{
	if(input->da > 0)
	{
		game->phase = GAME_PHASE_START;
	}
}

void update_game_line(Game_State *game)
{
	if(game->time >= game->hightlight_end_time)
	{
		clear_lines(game->board, WIDTH, HEIGHT, game->lines);
		game->line_count += game->pending_line_count; // before back to game phase
		game->points += compute_points(game->level, game->pending_line_count);

		s32 lines_for_the_next_level = get_lines_for_next_level(game->start_level, game->level);
		if(game->line_count >= lines_for_the_next_level)
		{
			++game->level;
		}

		game->phase = GAME_PHASE_PLAY; // back to game
	}
}

void update_game_play(Game_State *game, const Input_State *input)
{
	Block_State main_block = game->main_block; // copy state and validate below if its possible to change (avoid out of matrix bounds)
	Block_State second_block = game->second_block;

	if(input->dleft > 0) // input to move the piece to the left
	{
		if(!game->second_block.merged && !game->main_block.merged)
		{
			--second_block.offset_col;
			--main_block.offset_col;
		}
	}
	if(input->dright > 0) // input to move the piece to the right
	{
		if(!game->second_block.merged && !game->main_block.merged)
		{
			++second_block.offset_col;
			++main_block.offset_col;
		}
	}
	if(input->dup > 0) // rotate piece
	{
		if(!game->second_block.merged && !game->main_block.merged)
		{
			main_block.rotation = (main_block.rotation + 1) % 4;
		}
	}


	// validate before which piece is turned to out of bounds
	if((main_block.rotation == 1 && input->dright > 0) || (main_block.rotation == 3 && input->dleft > 0))
	{
		if(check_block_valid(&main_block, game->board, WIDTH, HEIGHT))
		{
			game->main_block = main_block; // update the in game piece state

			if(check_block_valid(&second_block, game->board, WIDTH, HEIGHT))
			{
				game->second_block = second_block; // update the in game piece state
			}
		}
	} else
	{
		if(check_block_valid(&second_block, game->board, WIDTH, HEIGHT))
		{
			game->second_block = second_block; // update the in game block state

			if(check_block_valid(&main_block, game->board, WIDTH, HEIGHT))
			{
				game->main_block = main_block; // update the in game block state
			}
		}
	}

	if(input->ddown > 0)
	{
		soft_drop(game);
	}

	if(input->da > 0)
	{
		while(soft_drop(game));
	}

	while(game->time >= game->next_drop_time) // while is not time to drop the piece
	{
		soft_drop(game);
	}

	game->pending_line_count = find_lines(game->board, WIDTH, HEIGHT, game->lines);
	if (game->pending_line_count > 0)
	{
		game->phase = GAME_PHASE_LINE; //switch game phase
		game->hightlight_end_time = game->time + 0.5f;
	}

	// game over
	s32 game_over_row = max(0, HEIGHT - VISIBLE_HEIGHT - 1);
	if(!check_row_empty(game->board, WIDTH, game_over_row))
	{
		game->phase = GAME_PHASE_GAMEOVER;
	}
}

void update_game(Game_State *game, const Input_State *input)
{
	switch (game->phase)
	{
		case GAME_PHASE_START:
			update_game_start(game, input);
			break;
		case GAME_PHASE_PLAY:
			return update_game_play(game, input);
			break;
		case GAME_PHASE_LINE:
			update_game_line(game);
			break;
		case GAME_PHASE_GAMEOVER:
			update_game_gameover(game, input);
			break;
	}
}

void render_game(const Game_State *game, SDL_Renderer *renderer, TTF_Font *font)
{
    char buffer[4096];
    
    Color highlight_color = color(0xFF, 0xFF, 0xFF, 0xFF);

    s32 margin_y = 100;
    
    draw_board(renderer, game->board, WIDTH, HEIGHT, 0, margin_y);

    if (game->phase == GAME_PHASE_PLAY)
    {
		Block_State main_block = game->main_block;
		Block_State second_block = game->second_block;
		
        draw_piece(renderer, &game->main_block, 0, margin_y);
        draw_piece(renderer, &game->second_block, 0, margin_y);

        while (check_block_valid(&main_block, game->board, WIDTH, HEIGHT))
        {
            main_block.offset_row++;
        }
        --main_block.offset_row;

        while (check_block_valid(&second_block, game->board, WIDTH, HEIGHT))
        {
			second_block.offset_row++;
        }
		--second_block.offset_row;
    }

    if (game->phase == GAME_PHASE_LINE)
    {
        for (s32 row = 0;
             row < HEIGHT;
             ++row)
        {
            if (game->lines[row])
            {
                s32 x = 0;
                s32 y = row * GRID_SIZE + margin_y;
                
                fill_rect(renderer, x, y,
                          WIDTH * GRID_SIZE, GRID_SIZE, highlight_color);
            }
        }
    }
    else if (game->phase == GAME_PHASE_GAMEOVER)
    {
        s32 x = WIDTH * GRID_SIZE / 2;
        s32 y = (HEIGHT * GRID_SIZE + margin_y) / 2;
        draw_string(renderer, font, "GAME OVER",
                    x, y, TEXT_ALIGN_CENTER, highlight_color);
    }
    else if (game->phase == GAME_PHASE_START)
    {
        s32 x = WIDTH * GRID_SIZE / 2;
        s32 y = (HEIGHT * GRID_SIZE + margin_y) / 2;
        draw_string(renderer, font, "PRESS START",
                    x, y, TEXT_ALIGN_CENTER, highlight_color);

        snprintf(buffer, sizeof(buffer), "STARTING LEVEL: %d", game->start_level);
        draw_string(renderer, font, buffer,
                    x, y + 30, TEXT_ALIGN_CENTER, highlight_color);
    }
    
    fill_rect(renderer,
              0, margin_y,
              WIDTH * GRID_SIZE, (HEIGHT - VISIBLE_HEIGHT) * GRID_SIZE,
              color(0x00, 0x00, 0x00, 0x00));
    

    snprintf(buffer, sizeof(buffer), "LEVEL: %d", game->level);
    draw_string(renderer, font, buffer, 5, 5, TEXT_ALIGN_LEFT, highlight_color);

    snprintf(buffer, sizeof(buffer), "LINES: %d", game->line_count);
    draw_string(renderer, font, buffer, 5, 35, TEXT_ALIGN_LEFT, highlight_color);
    
    snprintf(buffer, sizeof(buffer), "POINTS: %d", game->points);
    draw_string(renderer, font, buffer, 5, 65, TEXT_ALIGN_LEFT, highlight_color);
}
