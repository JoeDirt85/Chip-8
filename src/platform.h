#ifndef PLATFORM_H
#define PLATOFRM_H

#include <SDL3/SDL.h>
#include "chip_8.h"

class platform
{

private:

	SDL_Window* window{};
	SDL_Renderer* renderer{};
	SDL_Texture* texture{};

public:

	//class constructor
	platform(const char* title, int windowW, int windowH, int textureW, int textureH);

	//class deconstructor
	~platform();

	void update(void const* pixel, int pitch);		//method for updating display
	bool processInput(uint8_t* keys);				//method for processing input
};

#endif
