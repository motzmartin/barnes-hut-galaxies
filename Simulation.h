#pragma once

#include <SDL.h>
#include <sodium.h>

#include "Star.h"
#include "BarnesHut.h"

class Simulation
{
public:
	bool Initialize();
	bool PollEvents();
	void Update();
	void RenderFrame();
	void Destroy();

	void CreateStar(Vect3D pos, double mass);
	void CreateGalaxy(Vect3D pos,
		double radius,
		int starsNumber,
		double starsMass,
		double blackHoleMass);

	void ShowBoxes(Octree* octree);

private:
	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;

	std::vector<Star> stars;
	Octree* octree;
};

double RandomCoord(double coord, double radius);