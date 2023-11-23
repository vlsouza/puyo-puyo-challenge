#pragma once

#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "types.h"
#include "color.h"
#include "piece.h"
#include "level.h"
#include "label.h"

#define WIDTH 8 
#define HEIGHT 16
#define VISIBLE_HEIGHT 18 // to spawn the piece above the top of the grid 

#define ARRAY_COUNT(x) (sizeof(x) / sizeof((x)[0]))

struct Input_State
{
	u8 left;
	u8 right;
	u8 up;
	u8 down;

	u8 a;

    s8 dleft;
    s8 dright;
    s8 dup;
	s8 ddown;
	s8 da; //space bar
};

enum Game_Phase
{
	GAME_PHASE_START,
	GAME_PHASE_PLAY,
	GAME_PHASE_LINE, // this phase is the like a pause when lines are beeing removed
	GAME_PHASE_GAMEOVER
};

struct Game_State
{
	u8 board[WIDTH * HEIGHT]; //array of bit values 
	u8 lines[HEIGHT];
	s32 pending_line_count;

	// a piece is made from two blocks
	Block_State main_block; 
	Block_State second_block;

	Game_Phase phase;

	s32 start_level = 1;
	s32 level;
	s32 line_count;
	s32 points;

	f32 next_drop_time;
	f32 hightlight_end_time;
	f32 time;
};

using RenderInterface = void(*)(const Game_State *game, SDL_Renderer *renderer, TTF_Font *font);
using SpawnPieceInterface = void(*)(Game_State *game);
using UpdateInterface = void(*)(Game_State *game, const Input_State *input);

class Game {
public:
    Game();
    ~Game();

    void Run(RenderInterface render, UpdateInterface update, SpawnPieceInterface spawn_piece);

private:
    void InitSDL();
    void InitTTF();
    void SpawnPiece();
    bool SoftDrop();
    //void UpdateGameStart(const Input_State& input);
    //void UpdateGamePlay(const Input_State& input);
    //void UpdateGameLine();
    //void UpdateGameGameOver(const Input_State& input);
    //void UpdateGame(const Input_State& input);
    //void RenderGame();

    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;
    Game_State gameState;
    Input_State lastInputState;
    f32 deltaTime;
};

inline void 
merge_block(Game_State *game, Block_State block_state) {
    const Block *block = BLOCKS + block_state.block_index;

    for (s32 row = 0; row < block->side; ++row) {
        for (s32 col = 0; col < block->side; ++col) {
            u8 value = block_get(block, row, col, block_state.rotation);
            if (value) {
                s32 board_row = block_state.offset_row + row;
                s32 board_col = block_state.offset_col + col;
                matrix_set(game->board, WIDTH, board_row, board_col, value);
            }
        }
    }
}

// based on classic tetris from super nintendo
// represents the levels
const f32 FRAMES_PER_DROP[]= { 
	48,
	43,
	38,
	33,
	28,
	23,
	18,
	13,
	8,
	6,
	5,
	5,
	5,
	4,
	4,
	4,
	3,
	3,
	3,
	2,
	2,
	2,
	2,
	2,
	2,
	2,
	2,
	2,
	1
};

const f32 TARGET_SECONDS_PER_FRAME = 1.f / 60.f;

inline f32
get_time_to_next_drop(s32 level)
{
	if (level > 29) // total FRAMES_PER_DROP size means the levels
	{
		level = 29;
	}
	return FRAMES_PER_DROP[level] * TARGET_SECONDS_PER_FRAME;
}

inline void 
spawn_piece(Game_State *game)
{
    game->main_block = {};
	game->main_block.merged = false;
    game->main_block.block_index = (u8)random_int(0, ARRAY_COUNT(BLOCKS));
    game->main_block.offset_col = (WIDTH / 2) - 1;
    game->next_drop_time = game->time + get_time_to_next_drop(game->level);
	
	game->second_block = {};
	game->second_block.merged = false;
    game->second_block.block_index = (u8)random_int(0, ARRAY_COUNT(BLOCKS));
	game->second_block.offset_row = 1;
    game->second_block.offset_col = (WIDTH / 2) - 1;
    game->next_drop_time = game->time + get_time_to_next_drop(game->level);
}

inline bool
soft_drop(Game_State *game) // move the piece down after specific time
{
	if(!game->main_block.merged)
	{
		++game->main_block.offset_row;
	}
	if(!game->second_block.merged)
	{
		++game->second_block.offset_row;
	}

	if(game->main_block.rotation == 2) // secondary block is below main block
	{
		if (!check_block_valid(&game->main_block, game->board, WIDTH, HEIGHT) && !game->main_block.merged)
		{
			--game->main_block.offset_row;
			merge_block(game, game->main_block);
			game->main_block.merged = true;
		}

		if (!check_block_valid(&game->second_block, game->board, WIDTH, HEIGHT) && !game->second_block.merged)
		{
			--game->second_block.offset_row;
			merge_block(game, game->second_block);
			game->second_block.merged = true;
		}
	} else {
		if (!check_block_valid(&game->second_block, game->board, WIDTH, HEIGHT) && !game->second_block.merged)
		{
			--game->second_block.offset_row;
			merge_block(game, game->second_block);
			game->second_block.merged = true;
		}

		if (!check_block_valid(&game->main_block, game->board, WIDTH, HEIGHT) && !game->main_block.merged)
		{
			--game->main_block.offset_row;
			merge_block(game, game->main_block);
			game->main_block.merged = true;
		}
	}

	if(game->main_block.merged && game->second_block.merged)
	{
		spawn_piece(game);
		return false;
	}

    game->next_drop_time = game->time + get_time_to_next_drop(game->level);
    return true;
}

// game state

inline void 
update_game_start(Game_State *game, const Input_State *input)
{
	if(input->dup > 0)
	{
		++game->start_level;
	}
	if(input->ddown > 0 && game->start_level > 1)
	{
		--game->start_level;
	}
	if(input->da > 0) // press space bar
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

inline void 
update_game_gameover(Game_State *game, const Input_State *input)
{
	if(input->da > 0)
	{
		game->phase = GAME_PHASE_START;
	}
}

inline void 
update_game_line(Game_State *game)
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

inline void 
update_game_play(Game_State *game, const Input_State *input)
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

inline void 
update_game(Game_State *game, const Input_State *input)
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

inline void 
render_game(const Game_State *game, SDL_Renderer *renderer, TTF_Font *font)
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
