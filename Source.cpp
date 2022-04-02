#include "Simulation.h"

#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")
#pragma comment(lib, "libsodium.lib")

int SDL_main(int argc, char* argv[])
{
	Simulation simulation;

	if (simulation.Initialize())
	{
		while (simulation.PollEvents())
		{
			simulation.Update();
			simulation.RenderFrame();
		}

		simulation.Destroy();
	}

	return 0;
}