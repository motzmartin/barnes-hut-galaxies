#pragma once

#include <vector>
#include <cmath>

#define GRAVITY 0.00000000006674

struct Vect3D
{
	double x;
	double y;
	double z;
	Vect3D operator*(Vect3D v)
	{
		return { x * v.x, y * v.y, z * v.z };
	}
	Vect3D operator+(Vect3D v)
	{
		return { x + v.x, y + v.y, z + v.z };
	}
	Vect3D operator-(Vect3D v)
	{
		return { x - v.x, y - v.y, z - v.z };
	}
	Vect3D operator*(double d)
	{
		return { x * d, y * d, z * d };
	}
	Vect3D operator/(double d)
	{
		return { x / d, y / d, z / d };
	}
};

double Distance(Vect3D p1, Vect3D p2);

class Star
{
public:
	Vect3D GetPosition();
	void SetPosition(Vect3D newPosition);

	double GetMass();
	void SetMass(double newMass);

	int GetId();
	void SetId(int newId);

	Vect3D GetForce();
	void SetForce(Vect3D f);

	void Update(std::vector<Star> stars);

private:
	Vect3D force = { 0 };

	Vect3D position = { 0 };
	Vect3D lastPosition = { 0 };

	double mass = 0.0;

	int id = 0;
};