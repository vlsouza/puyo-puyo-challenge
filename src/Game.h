#pragma once

#include <iostream>
#include <cstdlib>
#include <cstdint>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include "types.h"
#include "colors.h"

#define WIDTH 8 //why 'define'?
#define HEIGHT 16
#define VISIBLE_HEIGHT 20 // because 2 the game uses to spawn the next puzzle piece 
#define GRID_SIZE 30

#define ARRAY_COUNT(x) (sizeof(x) / sizeof((x)[0]))

struct Piece_State
{
	u8 tetrino_index; 

	s32 offset_row; // where the piece currently is
	s32 offset_col;

	s32 rotation;

	bool merged;
};

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
    // Declarar outros métodos e membros privados conforme necessário
    void InitSDL();
    void InitTTF();
    void SpawnPiece();
    bool SoftDrop();
    //void MergePiece(const Piece_State& piece);
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