//                                         __          __                                 
//                                        |  \       _/  \                                
//  _______   ______    ______    ______  | $$____  |   $$    ______    ______   _______  
// /       \ |      \  /      \  /      \ | $$    \  \$$$$   /      \  /      \ |       \ 
//|  $$$$$$$  \$$$$$$\|  $$$$$$\|  $$$$$$\| $$$$$$$\  | $$  |  $$$$$$\|  $$$$$$\| $$$$$$$\
// \$$    \  /      $$| $$  | $$| $$  | $$| $$  | $$  | $$  | $$   \$$| $$    $$| $$  | $$
// _\$$$$$$\|  $$$$$$$| $$__/ $$| $$__/ $$| $$  | $$ _| $$_ | $$      | $$$$$$$$| $$  | $$
//|       $$ \$$    $$| $$    $$| $$    $$| $$  | $$|   $$ \| $$       \$$     \| $$  | $$
// \$$$$$$$   \$$$$$$$| $$$$$$$ | $$$$$$$  \$$   \$$ \$$$$$$ \$$        \$$$$$$$ \$$   \$$
//                    | $$      | $$                                                      
//                    | $$      | $$                                                      
//                     \$$       \$$                                                      
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define NANOSVG_IMPLEMENTATION
#include "nanosvg.h"
#define NANOSVGRAST_IMPLEMENTATION
#include "nanosvgrast.h"
#include <stdio.h>
#include <locale.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>


#define SDL_FLAGS (SDL_INIT_VIDEO | SDL_INIT_AUDIO)

static SDL_Window* wndw;
static SDL_Renderer* rndrr;
SDL_Texture* txtr;
SDL_Event ev;

typedef struct {
	float x, y, z;
	float u, v;
	int max_spd, event_type, type;
	float zoom, hungry;
} Player;
Player p;

//интерфейс
SDL_Texture* load_svg(const char* filename, float scale) {
	struct NSVGimage* image = nsvgParseFromFile(filename, "px", 96.0f);
	if (!image) return NULL;
	int w = (int)(image->width * scale);
	int h = (int)(image->height * scale);
	unsigned char* img_data = malloc(w * h * 4);
	struct NSVGrasterizer* rast = nsvgCreateRasterizer();
	nsvgRasterize(rast, image, 0, 0, scale, img_data, w, h, w * 4);
	SDL_Surface* surf = SDL_CreateSurfaceFrom(w, h, SDL_PIXELFORMAT_RGBA32, img_data, w * 4);
	SDL_Texture* tex = SDL_CreateTextureFromSurface(rndrr, surf);
	SDL_DestroySurface(surf);
	free(img_data);
	nsvgDeleteRasterizer(rast);
	nsvgDelete(image);
	return tex;
}
//спрайты
SDL_Texture* load_texture(const char* path) {
	int w, h, ch;
	unsigned char* pixels = stbi_load(path, &w, &h, &ch, 4);
	if (!pixels) {
		SDL_Log("STB Error: Не удалось найти файл %s. Причина: %s", path, stbi_failure_reason());
		return NULL;
	}
	SDL_Surface* surface = SDL_CreateSurfaceFrom(w, h, SDL_PIXELFORMAT_RGBA32, pixels, w * 4);
	if (!surface) {
		SDL_Log("SDL Surface Error: %s", SDL_GetError());
		stbi_image_free(pixels);
		return NULL;
	}
	SDL_Texture* texture = SDL_CreateTextureFromSurface(rndrr, surface);
	SDL_DestroySurface(surface);
	stbi_image_free(pixels);
	if (!texture) {
		SDL_Log("SDL Texture Error: %s", SDL_GetError());
		return NULL;
	}
	return texture;
}

int main(int argc, char* argv[]) {
	setlocale(LC_ALL, "Russian");
	bool kaka = false;
	if (!SDL_CreateWindowAndRenderer("Смешарики", 1920, 1080, SDL_WINDOW_RESIZABLE, &wndw, &rndrr)) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create window and renderer: %s", SDL_GetError());
		return 1;
	}
	if (!rndrr) {
		SDL_DestroyWindow(wndw);
		SDL_Quit();
		return 1;
	}
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
		return 1;
	}
	if (!SDL_Init(SDL_INIT_AUDIO)) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't initialize SDL: %s", SDL_GetError());
		return 1;
	}
	txtr = load_texture("resources/tree-kust.png");//SDL_CreateTextureFromSurface(rndrr, srfc);
	if (!txtr) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Couldn't create texture from surface: %s", SDL_GetError());
		return 3;
	}
	float tex_w, tex_h;
	SDL_GetTextureSize(txtr, &tex_w, &tex_h);
	SDL_FRect fr = { 0, 0, tex_w*2, tex_h*2 };
	
	while (!kaka) {
		while (SDL_PollEvent(&ev)) {
			if (ev.type == SDL_EVENT_QUIT) {kaka = true;}
		}
		SDL_SetRenderDrawColorFloat(rndrr, 0.8f, 0.0f, 0.0f, 1.0f);
		SDL_RenderClear(rndrr);
		SDL_RenderTexture(rndrr, txtr, NULL, &fr);
		SDL_RenderPresent(rndrr);
	}
	SDL_DestroyTexture(txtr);
	SDL_DestroyRenderer(rndrr);
	SDL_DestroyWindow(wndw);
	SDL_Quit();
	return 0;
}
