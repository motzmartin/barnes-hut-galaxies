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
	void InsertStar(std::vector<Star*>& stars, Star* star);
	void DeleteEmptyLeaves();
	MassData CalculateMasses();
	Vect3D CalculateForceOnStar(Star* star);

	Octree** GetNodes();

	int GetId();
	void SetId(int _id);

	int GetStarsNumber();
	void SetStarsNumber(int _starsNumber);

	Box GetBox();
	void SetBox(Box _box);

	MassData GetMassData();
	void SetMassData(MassData _massData);

private:
	int starsNumber = 0;
	int id = -1;
	MassData massData = { 0 };
	Octree* nodes[8] = { 0 };
	Box box = { 0 };
};

Octree* CreateOctree(std::vector<Star*>& stars);

void FreeOctree(Octree* octree);

bool IsInNode(Vect3D starPos, Box box);

void CalculateForces(std::vector<Star*>& stars, Octree* octree);