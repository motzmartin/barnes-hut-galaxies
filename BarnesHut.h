#pragma once

#include "Star.h"

struct MassData
{
	double mass;
	Vect3D centerOfMass;
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

	Vect3D GetPosition();
	void SetPosition(Vect3D _position);

	double GetMass();
	void SetMass(double _mass);

	Octree** GetNodes();
	void SetNode(int index, Octree* node);

	Box GetBox();
	void SetBox(Box _box);

	MassData GetMassData();
	void SetMassData(MassData _massData);

	bool IsLeaf();
	void SetLeaf(bool _isLeaf);

private:
	bool isLeaf = true;

	int starsNumber = 0;

	Vect3D position = { 0 };
	double mass = 0.0;

	Octree* nodes[8] = { 0 };

	Box box = { 0 };

	MassData massData = { 0 };
};

Octree* CreateOctree(std::vector<Star>& stars);

void DeleteEmptyLeaves(Octree* octree);

bool IsInNode(Vect3D starPos, Box box);

void CreateNodes(Octree* octree, Box box, double newSize);

void InsertStar(std::vector<Star>& stars, Star star, Octree* octree);

void FreeOctree(Octree* octree);

MassData CalculateMasses(Octree* octree);

void CalculateForces(std::vector<Star>& stars, Octree* octree);

Vect3D Force(Vect3D pos, double mass, Vect3D targetPos, double targetMass);

Vect3D CalculateForceOnStar(Star* star, Octree* node);