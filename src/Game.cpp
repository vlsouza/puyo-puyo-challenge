#include <functional>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "color.h"
#include "types.h"
#include "block.h"
#include "game.h"

Game::Game() : window(nullptr), renderer(nullptr), font(nullptr) {
    InitSDL();
    InitTTF();
}

Game::~Game() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_Quit();
}

void Game::InitSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        std::exit(1);
    }

    this->window = SDL_CreateWindow(
        "Puyo Puyo Challenge",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        295,
        690,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    if (!this->window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        std::exit(1);
    }

    this->renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (!this->renderer) {
        std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        std::exit(1);
    }
}

void Game::InitTTF() {
    if (TTF_Init() < 0) {
        std::cerr << "SDL_ttf initialization failed: " << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(this->renderer);
        SDL_DestroyWindow(this->window);
        SDL_Quit();
        std::exit(1);
    }

    const char* fontName = "./build/Pokemon Classic.ttf";
    this->font = TTF_OpenFont(fontName, 15);

    if (!this->font) {
        std::cerr << "Font loading failed: " << TTF_GetError() << std::endl;
        TTF_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        std::exit(1);
    }
}

//void Game::UpdateGameStart(const Input_State& input) {
//    // Implemente conforme necessário
//}
//
//void Game::UpdateGamePlay(const Input_State& input) {
//    // Implemente conforme necessário
//}
//
//void Game::UpdateGameLine() {
//    // Implemente conforme necessário
//}
//
//void Game::UpdateGameGameOver(const Input_State& input) {
//    // Implemente conforme necessário
//}
//
//void Game::UpdateGame(const Input_State& input) {
//    // Implemente conforme necessário
//}
//
//void Game::RenderGame() {
//    // Implemente conforme necessário
//}

void Game::Run(RenderInterface render, UpdateInterface update, SpawnPieceInterface spawn_piece) {
    Game_State game = {}; // initialize
	Input_State input = {}; // initialize

    spawn_piece(&game);

    bool quit = false;

    while (!quit) {
        deltaTime = SDL_GetTicks() / 1000.0f;
        game.time = SDL_GetTicks() / 1000.0f;
        SDL_Event e;
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }
    
        s32 key_count;
        const u8 *key_states = SDL_GetKeyboardState(&key_count);

        if (key_states[SDL_SCANCODE_ESCAPE])
        {
            quit = true;
        }
        
        Input_State prev_input = input;
        
        input.left = key_states[SDL_SCANCODE_LEFT];
        input.right = key_states[SDL_SCANCODE_RIGHT];
        input.up = key_states[SDL_SCANCODE_UP];
        input.down = key_states[SDL_SCANCODE_DOWN];
        input.a = key_states[SDL_SCANCODE_SPACE];

        input.dleft = input.left - prev_input.left;
        input.dright = input.right - prev_input.right;
        input.ddown = input.down - prev_input.down;
        input.dup = input.up - prev_input.up;
        input.da = input.a - prev_input.a;
        
        SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 0);
        SDL_RenderClear(this->renderer);
        
        update(&game, &input);
        render(&game, this->renderer, this->font);

        SDL_RenderPresent(renderer);
    }

    TTF_CloseFont(font);
	SDL_DestroyRenderer(renderer);
    SDL_Quit();
}
