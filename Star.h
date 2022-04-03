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

class Star
{
public:
	Vect3D GetPosition();
	void SetPosition(Vect3D _position);

	double GetMass();
	void SetMass(double _mass);

	int GetId();
	void SetId(int _id);

	Vect3D GetForce();
	void SetForce(Vect3D _force);

	void Update(std::vector<Star> stars);

private:
	Vect3D force = { 0 };

	Vect3D position = { 0 };
	Vect3D lastPosition = { 0 };

	double mass = 0.0;

	int id = 0;
};

double Distance(Vect3D p1, Vect3D p2);