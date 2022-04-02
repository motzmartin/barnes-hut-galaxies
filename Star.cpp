#include "Star.h"

Vect3D Star::GetPosition()
{
	return position;
}

void Star::SetPosition(Vect3D newPosition)
{
	lastPosition = newPosition;
	position = newPosition;
}

double Star::GetMass()
{
	return mass;
}

void Star::SetMass(double newMass)
{
	mass = newMass;
}

int Star::GetId()
{
	return id;
}

void Star::SetId(int newId)
{
	id = newId;
}

Vect3D Star::GetForce()
{
	return force;
}

void Star::SetForce(Vect3D f)
{
	force = f;
}

void Star::Update(std::vector<Star> stars)
{
	Vect3D tmp = lastPosition;
	lastPosition = position;

	position = position * 2.0 - tmp + force / mass;
}

double Distance(Vect3D p1, Vect3D p2)
{
	return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2) + pow(p1.z - p2.z, 2));
}