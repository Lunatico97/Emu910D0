
#include <renderer.hpp>

Renderer::Renderer(const char *title, int width, int height)
{
	SDL_Init(SDL_INIT_EVERYTHING) ;
	window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0) ;
	if(window == nullptr) std::cout << SDL_GetError() << std::endl ;
	ren = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC) ;
	if(ren == nullptr) std::cout << SDL_GetError() << std::endl ;
	scrW = width ;
	scrH = height ;
	std::cout << "!!! SDL 2.0 Initialized Successfully !!!" << std::endl ;
}

SDL_Texture *Renderer::loadTexture(const int width, const int height)
{
	SDL_Texture *tex = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, width, height);
	if(tex == NULL) std::cout << SDL_GetError() << std::endl ;
	return tex;
}

void Renderer::freeTex(SDL_Texture*t)
{
	SDL_DestroyTexture(t) ;
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

void Renderer::renderPt(int x, int y)
{
	SDL_RenderDrawPoint(ren, x, y) ;	
}

void Renderer::renderRect(SDL_Rect rect, bool fill)
{
	if(fill) SDL_RenderFillRect(ren, &rect) ;
	else SDL_RenderDrawRect(ren, &rect) ;	
}

void Renderer::renderRect(SDL_Rect rect, Uint8 r, Uint8 g, Uint8 b, bool fill)
{
	SDL_SetRenderDrawColor(ren, r, g, b, 255) ;
	if(fill) SDL_RenderFillRect(ren, &rect) ;
	else SDL_RenderDrawRect(ren, &rect) ;	
}

void Renderer::renderFrame(SDL_Rect rect, SDL_Texture* frame_tex, u32* frame_buffer, int frame_pitch)
{
	SDL_UpdateTexture(frame_tex, NULL, frame_buffer, frame_pitch*sizeof(u32));
	SDL_RenderCopy(ren, frame_tex, NULL, &rect);
}

void Renderer::setColor(Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{
	SDL_SetRenderDrawColor(ren, r, g, b, a) ;
}

void Renderer::cleanRenderer()
{
	SDL_DestroyRenderer(ren) ;
	SDL_DestroyWindow(window) ;
	SDL_Quit() ;
	std::cout << "!!! SDL 2.0 Quitted Successfully !!!" << std::endl ;
}

void Renderer::init_wxs()
{
	ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForSDLRenderer(window, ren);
    ImGui_ImplSDLRenderer2_Init(ren);
}

void Renderer::set_frame()
{
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();
}

void Renderer::fit_frame()
{
    ImGui::Render();
	ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), ren);
}

void Renderer::cleanup()
{
    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}
