#include <cstdlib>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <cassert>

#include <SDL2/SDL.h> // platform layer
#include <SDL2/SDL_ttf.h>

#include "game.h"

int main(int argc, char *argv[]) {
	Game* game = new Game();
	game->Run(render_game, update_game, spawn_piece);

	return 0;    
}