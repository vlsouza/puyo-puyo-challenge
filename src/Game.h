#pragma once

#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "types.h"
#include "color.h"
#include "piece.h"

#define WIDTH 8 //why 'define'?
#define HEIGHT 16
#define VISIBLE_HEIGHT 20 // because 2 the game uses to spawn the next puzzle piece 

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
	s8 da;
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
	u8 board[WIDTH * HEIGHT]; //array of bit values (pesquisar para saber mais)
	u8 lines[HEIGHT];
	s32 pending_line_count;

	Piece_State piece;

	Piece_State piece2;

	Game_Phase phase;

	s32 start_level;
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
merge_piece(Game_State *game, Piece_State piece) {
    const Block *block = BLOCKS + piece.block_index;

    for (s32 row = 0; row < block->side; ++row) {
        for (s32 col = 0; col < block->side; ++col) {
            u8 value = block_get(block, row, col, piece.rotation);
            if (value) {
                s32 board_row = piece.offset_row + row;
                s32 board_col = piece.offset_col + col;
                matrix_set(game->board, WIDTH, board_row, board_col, value);
            }
        }
    }
}

// based on classic tetrin from super nintendo
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
    game->piece = {};
	game->piece.merged = false;
    game->piece.block_index = (u8)random_int(0, ARRAY_COUNT(BLOCKS));
    game->piece.offset_col = WIDTH / 2;
    game->next_drop_time = game->time + get_time_to_next_drop(game->level);
	
	game->piece2 = {};
	game->piece2.merged = false;
    game->piece2.block_index = (u8)random_int(0, ARRAY_COUNT(BLOCKS));
	game->piece2.offset_row = 1;
    game->piece2.offset_col = WIDTH / 2;
    game->next_drop_time = game->time + get_time_to_next_drop(game->level);
}

inline bool
soft_drop(Game_State *game) // move the piece down after specific time
{
	if(!game->piece.merged)
	{
		++game->piece.offset_row;
	}
	if(!game->piece2.merged)
	{
		++game->piece2.offset_row;
	}

	if(game->piece.rotation == 2) // secondary piece is below main piece
	{
		if (!check_piece_valid(&game->piece, game->board, WIDTH, HEIGHT) && !game->piece.merged)
		{
			--game->piece.offset_row;
			merge_piece(game, game->piece);
			game->piece.merged = true;
		}

		if (!check_piece_valid(&game->piece2, game->board, WIDTH, HEIGHT) && !game->piece2.merged)
		{
			--game->piece2.offset_row;
			merge_piece(game, game->piece2);
			game->piece2.merged = true;
		}
	} else {
		if (!check_piece_valid(&game->piece2, game->board, WIDTH, HEIGHT) && !game->piece2.merged)
		{
			--game->piece2.offset_row;
			merge_piece(game, game->piece2);
			game->piece2.merged = true;
		}

		if (!check_piece_valid(&game->piece, game->board, WIDTH, HEIGHT) && !game->piece.merged)
		{
			--game->piece.offset_row;
			merge_piece(game, game->piece);
			game->piece.merged = true;
		}
	}

	if(game->piece.merged && game->piece2.merged)
	{
		spawn_piece(game);
		return false;
	}

    game->next_drop_time = game->time + get_time_to_next_drop(game->level);
    return true;
}