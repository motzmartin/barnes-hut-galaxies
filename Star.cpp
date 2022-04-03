#include "Star.h"

Vect3D Star::GetPosition()
{
	return position;
}

void Star::SetPosition(Vect3D _position)
{
	lastPosition = _position;
	position = _position;
}

double Star::GetMass()
{
	return mass;
}

void Star::SetMass(double _mass)
{
	mass = _mass;
}

int Star::GetId()
{
	return id;
}

void Star::SetId(int _id)
{
	id = _id;
}

Vect3D Star::GetForce()
{
	return force;
}

void Star::SetForce(Vect3D _force)
{
	force = _force;
}

void Star::Update()
{
	Vect3D tmp = position;

	position = position * 2.0 - lastPosition + force / mass;

	lastPosition = tmp;
}

double Distance(Vect3D p1, Vect3D p2)
{
	return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2) + pow(p1.z - p2.z, 2));
}