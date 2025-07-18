
#include <renderer.hpp>

Renderer::Renderer(const char *title, int width, int height)
{
	SDL_Init(SDL_INIT_EVERYTHING) ;
	TTF_Init() ;
	IMG_Init(IMG_INIT_PNG) ;
	window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0) ;
	if(window == nullptr) std::cout << SDL_GetError() << std::endl ;
	ren = SDL_CreateRenderer(window, -1, 0) ;
	if(ren == nullptr) std::cout << SDL_GetError() << std::endl ;
	scrW = width ;
	scrH = height ;
	std::cout << "!!! SDL 2.0 Initialized Successfully !!!" << std::endl ;
}

SDL_Texture *Renderer::loadTexture(const char *location)
{
	SDL_Surface *surface ;
	surface = IMG_Load(location) ;
	if(surface == NULL) std::cout << IMG_GetError() << std::endl ;
	SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, surface) ;
	SDL_FreeSurface(surface) ;
	return tex ;
}

SDL_Texture* Renderer::loadTextureCK(const char *path, Uint8 r, Uint8 g, Uint8 b)
{
	SDL_Surface *surface = IMG_Load(path) ;
	SDL_SetColorKey(surface, SDL_TRUE, SDL_MapRGB(surface->format, r, g, b)) ;
    SDL_Texture *texture = SDL_CreateTextureFromSurface(ren, surface) ;
	SDL_FreeSurface(surface) ;
	return texture ;
}

TTF_Font *Renderer::loadFont(const char *location, int pt)
{
	TTF_Font* f = TTF_OpenFont(location, pt) ;
	if(f == NULL) std::cout << TTF_GetError() << std::endl ;
	return f;
}

void Renderer::freeTex(SDL_Texture*t)
{
	SDL_DestroyTexture(t) ;
}

void Renderer::freeFont(TTF_Font *f)
{
	TTF_CloseFont(f) ;
}

void Renderer::clear()
{
	SDL_RenderClear(ren) ;
}

void Renderer::render(int x, int y, SDL_Texture *tex)
{
	SDL_Rect src, dest ;
	src.x = 0 ;
	src.y = 0 ;
	SDL_QueryTexture(tex, NULL, NULL, &src.w, &src.h) ;
	dest = {x, y, src.w, src.h} ;
	SDL_RenderCopy(ren, tex, &src, &dest) ;	
}

SDL_Texture* Renderer::loadText(const char *text, TTF_Font *font, SDL_Color color) 
{
	SDL_Surface *surface = TTF_RenderText_Blended(font, text, color) ;
	SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, surface) ;
	if(tex == NULL) std::cout << TTF_GetError() << std::endl ;
	SDL_FreeSurface(surface) ;
	return tex ;
}

void Renderer::render(SDL_Texture *t, SDL_Rect des)
{
	SDL_RenderCopy(ren, t, NULL, &des) ;
}

void Renderer::render(SDL_Texture *t, SDL_Rect src, SDL_Rect des)
{
	SDL_RenderCopy(ren, t, &src, &des) ;
}

void Renderer::renderEx(SDL_Texture *t, SDL_Rect src, SDL_Rect des, float angle, SDL_RendererFlip flipMode, SDL_Point point)
{
	SDL_RenderCopyEx(ren, t, &src, &des, angle, &point, flipMode) ;
}

void Renderer::display()
{
	SDL_RenderPresent(ren) ;
}

void Renderer::renderRect(SDL_Rect rect, Uint8 r, Uint8 g, Uint8 b, bool fill)
{
	SDL_SetRenderDrawColor(ren, r, g, b, 255) ;
	SDL_RenderDrawRect(ren, &rect) ;	
	if(fill) SDL_RenderFillRect(ren, &rect) ;
}

void Renderer::setColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	SDL_SetRenderDrawColor(ren, r, g, b, a) ;
}

void Renderer::cleanRenderer()
{
	SDL_DestroyRenderer(ren) ;
	SDL_DestroyWindow(window) ;
	TTF_Quit() ;
	IMG_Quit() ;
	SDL_Quit() ;
	std::cout << "!!! SDL 2.0 Quitted Successfully !!!" << std::endl ;
}