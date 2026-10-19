#include "platform.h"

platform::platform(const char* title, int windowW, int windowH, int textureW, int textureH) {
	 
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow(title, windowW, windowH, 0);
	renderer = SDL_CreateRenderer(window, NULL);
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING,textureW, textureH);
	SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);
}

platform::~platform() {

	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void platform::update(void const* pixel, int pitch) {

	SDL_UpdateTexture(texture, NULL, pixel, pitch);
	SDL_RenderClear(renderer);
	SDL_RenderTexture(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}

bool platform::processInput(uint8_t* key) {

	bool running = true;

	SDL_Event event;

		while (SDL_PollEvent(&event)) {

			switch (event.type) {

			case (SDL_EVENT_QUIT):
	
				running = false;
				break;

			case SDL_EVENT_KEY_DOWN:

				switch (event.key.key)
				{
				case SDLK_ESCAPE:
					running = false;
					break;

				case SDLK_1:
					key[1] = 1;
					break;

				case SDLK_2:
					key[2] = 1;
					break;

				case SDLK_3:
					key[3] = 1;
					break;
				case SDLK_4:
					key[0xC] = 1;
					break;

				case SDLK_Q:
					key[4] = 1;
					break;

				case SDLK_W:
					key[5] = 1;
					break;

				case SDLK_E:
					key[6] = 1;
					break;

				case SDLK_R:
					key[0xD] = 1;
					break;

				case SDLK_A:
					key[7] = 1;
					break;

				case SDLK_S:
					key[8] = 1;
					break;

				case SDLK_D:
					key[9] = 1;
					break;
					 
				case SDLK_F:
					key[0xE] = 1;
					break;

				case SDLK_Z:
					key[0xA] = 1;
					break;

				case SDLK_X:
					key[0] = 1;
					break;

				case SDLK_C:
					key[0xB] = 1;
					break;

				case SDLK_V:
					key[0xF] = 1;
					break;
				}
				break;

			case SDL_EVENT_KEY_UP:

				switch (event.key.key)
				{
				case SDLK_1:
					key[1] = 0;
					break;

				case SDLK_2:
					key[2] = 0;
					break;

				case SDLK_3:
					key[3] = 0;
					break;
				case SDLK_4:
					key[0xC] = 0;
					break;

				case SDLK_Q:
					key[4] = 0;
					break;

				case SDLK_W:
					key[5] = 0;
					break;

				case SDLK_E:
					key[6] = 0;
					break;

				case SDLK_R:
					key[0xD] = 0;
					break;

				case SDLK_A:
					key[7] = 0;
					break;

				case SDLK_S:
					key[8] = 0;
					break;

				case SDLK_D:
					key[9] = 0;
					break;

				case SDLK_F:
					key[0xE] = 0;
					break;

				case SDLK_Z:
					key[0xA] = 0;
					break;

				case SDLK_X:
					key[0] = 0;
					break;

				case SDLK_C:
					key[0xB] = 0;
					break;

				case SDLK_V:
					key[0xF] = 0;
					break;
				}
				break;
			}
		}
		return running;
	}
 