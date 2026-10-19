#include "chip_8.h"
#include "platform.h"
#include <string>
#include <iostream>


const int Screen_Width = 64;
const int Screen_Height = 32;

int videoscaler = 20;


int main(int argc, char** argv) {
	
	chip_8 myChip8;

	if (argc != 2) {
		std::cout << "Usage: " << argv[0] << " <ROM File>";
		return 1;
	}

	char const* ROMFileName = argv[1];

	//call to create and open window
	platform myPlatform("CHIP 8 Emulator", Screen_Width * videoscaler, Screen_Height * videoscaler, Screen_Width, Screen_Height);

	//attempts to load rom into memory
	if (!myChip8.loadROM(ROMFileName)) {
		return 2;
	};

	const int target_cpu_hz = 700;		//defines and initializes cpu speed
	const int target_fps = 60;			//defines and initializes target fps

	const int cyclesPerFrame = target_cpu_hz / target_fps;			//calculates and assigns cycles per frame
	const int frameDuration = 1000 / target_fps;					//calculates the duration of frames
	int videoPitch = sizeof(myChip8.display[0]) * Screen_Width;		//gets the number of bytes to advance rows
		
	while (true) {

		//gets the time at the start of frame
		uint32_t frameStartTime = SDL_GetTicks();
		
		//main game loop 
		if (myPlatform.processInput(myChip8.keypad)) {

			//executes cycles for current frame
			for (int i = 0; i < cyclesPerFrame; ++i) {
				myChip8.emulateCycle();
			}

			//updates timers
			myChip8.updateTimers();

			//checks drawflags, updates screen if needed
			if (myChip8.drawFlag) {
				
				myChip8.drawFlag = false;

				myPlatform.update(myChip8.display, videoPitch);
			}

			//calculates the current frame time
			uint32_t frameProcessTime = SDL_GetTicks() - frameStartTime;

			//checks if frame processed faster than the set duration 
			if (frameDuration > frameProcessTime) {

				SDL_Delay(frameDuration - frameProcessTime);
			}
		}
		else {
			break;
		}
	}

	return 0;
}