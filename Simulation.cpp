#include "Simulation.h"

bool Simulation::Initialize()
{
	if (sodium_init() < 0)
	{
		return false;
	}

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		return false;
	}

	window = SDL_CreateWindow("Simulation",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		600,
		600,
		SDL_WINDOW_SHOWN);

	if (window == nullptr)
	{
		return false;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	if (renderer == nullptr)
	{
		return false;
	}

	CreateGalaxy({ 200.0, 200.0, 200.0 }, 100.0, 1000, 1.0E9, 1.0E12);
	CreateGalaxy({ 400.0, 400.0, 400.0 }, 100.0, 1000, 1.0E9, 1.0E12);

	return true;
}

void Simulation::CreateStar(Vect3D pos, double mass)
{
	Star* star = new Star;

	star->SetPosition(pos);
	star->SetMass(mass);
	star->SetId(stars.size());

	stars.push_back(star);
}

void Simulation::CreateGalaxy(Vect3D pos,
	double radius,
	int starsNumber,
	double starsMass,
	double blackHoleMass)
{
	CreateStar(pos, blackHoleMass);

	for (int i = 0; i < starsNumber; i++)
	{
		Vect3D starPos;
		do
		{
			starPos =
			{
				RandomCoord(pos.x, radius),
				RandomCoord(pos.y, radius),
				RandomCoord(pos.z, radius)
			};
		} while (Distance(pos, starPos) > radius);

		CreateStar(starPos, starsMass);
	}
}

bool Simulation::PollEvents()
{
	SDL_Event SDLEvent;

	while (SDL_PollEvent(&SDLEvent))
	{
		switch (SDLEvent.type)
		{
		case SDL_QUIT:
			return false;
		}
	}

	return true;
}

void Simulation::Update()
{
	if (octree != nullptr)
	{
		octree->Free();
	}

	octree = CreateOctree(stars);

	octree->CalculateMasses();

	CalculateForces(stars, octree);

	for (int i = 0; i < stars.size(); i++)
	{
		stars[i]->Update();
	}
}

void Simulation::ShowBoxes(Octree* octree)
{
	if (octree->GetStarsNumber() > 0)
	{
		Octree** nodes = octree->GetNodes();

		for (int i = 0; i < 8; i++)
		{
			if (nodes[i] != nullptr)
			{
				ShowBoxes(nodes[i]);
			}
		}
	}

	Box b = octree->GetBox();

	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	SDL_RenderDrawLine(renderer, (int)b.pos.x, (int)b.pos.y, (int)(b.pos.x + b.size), (int)b.pos.y);
	SDL_RenderDrawLine(renderer, (int)b.pos.x, (int)b.pos.y, (int)b.pos.x, (int)(b.pos.y + b.size));
	SDL_RenderDrawLine(renderer, (int)(b.pos.x + b.size), (int)b.pos.y, (int)(b.pos.x + b.size), (int)(b.pos.y + b.size));
	SDL_RenderDrawLine(renderer, (int)b.pos.x, (int)(b.pos.y + b.size), (int)(b.pos.x + b.size), (int)(b.pos.y + b.size));
}

void Simulation::RenderFrame()
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	void* tmp;
	Uint32* pixels;
	SDL_PixelFormat* format;
	int pitch;

	SDL_Texture* texture = SDL_CreateTexture(renderer,
		SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_STREAMING,
		600,
		600);

	format = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888);

	SDL_LockTexture(texture, nullptr, &tmp, &pitch);

	pixels = (Uint32*)tmp;

	for (int i = 0; i < stars.size(); i++)
	{
		Vect3D starPosition = stars[i]->GetPosition();

		if (starPosition.x >= 0.0 &&
			starPosition.x < 600.0 &&
			starPosition.y >= 0.0 &&
			starPosition.y < 600.0)
		{
			int grey = (int)(starPosition.z / 600.0 * 255.0);

			if (grey > 0)
			{
				if (grey > 255)
				{
					grey = 255;
				}
				Uint32 rgba = SDL_MapRGBA(format, grey, grey, grey, 0);
				pixels[(int)(starPosition.y) * 600 + (int)(starPosition.x)] = rgba;
			}
		}
	}

	SDL_FreeFormat(format);

	SDL_UnlockTexture(texture);

	SDL_RenderCopy(renderer, texture, nullptr, nullptr);

	SDL_DestroyTexture(texture);

	//ShowBoxes(octree);

	SDL_RenderPresent(renderer);
}

void Simulation::Destroy()
{
	for (int i = 0; i < stars.size(); i++)
	{
		delete stars[i];
	}

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);

	SDL_Quit();
}

double RandomCoord(double coord, double radius)
{
	return coord - radius + (double)(randombytes_uniform((int)radius * 2));
}