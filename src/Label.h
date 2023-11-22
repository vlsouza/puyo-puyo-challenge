#ifndef LABEL_H
#define LABEL_H

#include <SDL2/SDL.h> 
#include <SDL2/SDL_ttf.h>

#include "types.h"
#include "color.h"

enum Text_Align
{
	TEXT_ALIGN_LEFT,
	TEXT_ALIGN_CENTER,
	TEXT_ALIGN_RIGHT
};

void
draw_string(SDL_Renderer *renderer,
            TTF_Font *font,
            const char *text,
            s32 x, s32 y,
            Text_Align alignment,
            Color color)
{
    SDL_Color sdl_color = SDL_Color { color.r, color.g, color.b, color.a };
    SDL_Surface *surface = TTF_RenderText_Solid(font, text, sdl_color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect rect;
    rect.y = y;
    rect.w = surface->w;
    rect.h = surface->h;
    switch (alignment)
    {
    case TEXT_ALIGN_LEFT:
        rect.x = x;
        break;
    case TEXT_ALIGN_CENTER:
        rect.x = x - surface->w / 2;
        break;
    case TEXT_ALIGN_RIGHT:
        rect.x = x - surface->w;
        break;
    }

    SDL_RenderCopy(renderer, texture, 0, &rect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

#endif // LABEL_H
