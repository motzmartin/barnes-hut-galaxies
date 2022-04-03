#pragma once

#include "Star.h"

struct MassData
{
	double mass;
	Vect3D position;
};

struct Box
{
	Vect3D pos;
	double size;
};

class Octree
{
public:
	int GetStarsNumber();
	void SetStarsNumber(int _starsNumber);

	Octree** GetNodes();
	void SetNode(int index, Octree* node);

	Box GetBox();
	void SetBox(Box _box);

	MassData GetMassData();
	void SetMassData(MassData _massData);

private:
	int starsNumber = 0;

	Octree* nodes[8] = { 0 };

	Box box = { 0 };

	MassData massData = { 0 };
};

Octree* CreateOctree(std::vector<Star>& stars);

void InsertStar(std::vector<Star>& stars, Star star, Octree* octree);

void DeleteEmptyLeaves(Octree* octree);

MassData CalculateMasses(Octree* octree);

void CalculateForces(std::vector<Star>& stars, Octree* octree);

Vect3D CalculateForceOnStar(Star* star, Octree* node);

Vect3D Force(Vect3D pos, double mass, Vect3D targetPos, double targetMass, double distance);

bool IsInNode(Vect3D starPos, Box box);

void FreeOctree(Octree* octree);