#include <global.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_sdlrenderer2.h>

#ifndef __RENDERER_H__
#define __RENDERER_H__

/* 
	This is my simple goto wrapper that I have made after using SDL2 a lot so that
	I can make something happen quickly using these tasty functions right at my disposal !
	Also, I removed SDL2_TTF & SDL2_IMAGE related functions from this as, we are not using them here !
	[ 
	  Note: There is no reason to dive deep into SDL2 for this ! 
	  Please checkout SDL2 documentation if you are interested but, unfamiliar with these callbacks !
	]
*/
class Renderer
{
    public:
	    SDL_Window* window;
        SDL_Renderer *ren;
        Renderer(const char *title, int width, int height);
		SDL_Texture *loadTexture(const int width, const int height);
		void freeTex(SDL_Texture *t);
		void cleanRenderer();
		void render(SDL_Texture *t, SDL_Rect des);
		void render(int x, int y, SDL_Texture *tex);
		void render(SDL_Texture *t, SDL_Rect src, SDL_Rect des);
		void renderEx(SDL_Texture *t, SDL_Rect src, SDL_Rect des, float angle, SDL_RendererFlip flipMode, SDL_Point point);
		void renderPt(int x, int y);
		void renderRect(SDL_Rect rect, bool fill);
		void renderRect(SDL_Rect rect, Uint8 r, Uint8 g, Uint8 b, bool fill);
		void renderFrame(SDL_Rect rect, SDL_Texture* frame_tex, u32* frame_buffer, int frame_pitch);
		void setColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a);
		void display();
		void clear();

		// Widget &  UI [IMGUI Integration with SDL2 Backend]
		void init_wxs();
		void set_frame();
		void fit_frame();
		void cleanup();

    private:
		int scrW, scrH;
};

#endif
