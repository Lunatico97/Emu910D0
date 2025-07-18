#include <utils.hpp>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#ifndef __RENDERER_H__
#define __RENDERER_H__

/* 
	This is my simple goto wrapper that I have made after using SDL2 a lot so that
	I can make something happen quickly using these tasty functions right at my disposal !
	[ 
	  Note: There is no reason to dive deep into SDL2 for this ! 
	  Please checkout SDL2 documentation if you are interested but, unfamiliar with these callbacks !
	]
*/
class Renderer
{
    public:
        Renderer(const char *title, int width, int height);
		SDL_Texture *loadTexture(const char *location);
		SDL_Texture* loadTextureCK(const char *path, Uint8 r, Uint8 g, Uint8 b);
		SDL_Texture* loadText(const char *text, TTF_Font *font, SDL_Color color);
		TTF_Font *loadFont(const char *location, int pt);
		void freeFont(TTF_Font *font);
		void freeTex(SDL_Texture *t);
		void cleanRenderer();
		void render(SDL_Texture *t, SDL_Rect des);
		void render(int x, int y, SDL_Texture *tex);
		void render(SDL_Texture *t, SDL_Rect src, SDL_Rect des);
		void renderEx(SDL_Texture *t, SDL_Rect src, SDL_Rect des, float angle, SDL_RendererFlip flipMode, SDL_Point point);
		void renderRect(SDL_Rect rect, Uint8 r, Uint8 g, Uint8 b, bool fill);
		void setColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
		void display();
		void clear();

    private:
        SDL_Window* window;
        SDL_Renderer *ren;
		int scrW, scrH;
};

#endif