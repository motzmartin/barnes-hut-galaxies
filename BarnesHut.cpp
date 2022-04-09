#include "BarnesHut.h"

void Octree::InsertStar(std::vector<Star*>& stars, Star* star)
{
	if (starsNumber > 1)
	{
		for (int i = 0; i < 8; i++)
		{
			if (IsInNode(star->GetPosition(), nodes[i]->GetBox()))
			{
				nodes[i]->InsertStar(stars, star);

				break;
			}
		}
	}
	else if (starsNumber == 1)
	{
		double newSize = box.size / 2.0;

		Vect3D positions[8] =
		{
			box.pos,
			{ box.pos.x + newSize, box.pos.y, box.pos.z },
			{ box.pos.x, box.pos.y + newSize, box.pos.z },
			{ box.pos.x, box.pos.y, box.pos.z + newSize },
			{ box.pos.x + newSize, box.pos.y + newSize, box.pos.z },
			{ box.pos.x + newSize, box.pos.y, box.pos.z + newSize },
			{ box.pos.x, box.pos.y + newSize, box.pos.z + newSize },
			{ box.pos.x + newSize, box.pos.y + newSize, box.pos.z + newSize }
		};

		bool existingStarSwitched = false;
		bool starInserted = false;

		for (int i = 0; i < 8; i++)
		{
			Octree* newNode = new Octree;

			newNode->SetBox({ positions[i], newSize });

			if (!existingStarSwitched && IsInNode(massData.position, newNode->GetBox()))
			{
				newNode->SetMassData(massData);
				newNode->SetId(id);
				newNode->SetStarsNumber(1);

				massData = { 0 };
				id = -1;

				existingStarSwitched = true;
			}

			if (!starInserted && IsInNode(star->GetPosition(), newNode->GetBox()))
			{
				newNode->InsertStar(stars, star);

				starInserted = true;
			}

			nodes[i] = newNode;
		}
	}
	else
	{
		massData = { star->GetMass(), star->GetPosition() };
		id = star->GetId();
	}

	starsNumber += 1;
}

void Octree::DeleteEmptyLeaves()
{
	if (starsNumber == 1)
	{
		return;
	}

	for (int i = 0; i < 8; i++)
	{
		if (nodes[i]->GetStarsNumber() == 0)
		{
			delete nodes[i];
			nodes[i] = nullptr;
		}
		else
		{
			nodes[i]->DeleteEmptyLeaves();
		}
	}
}

MassData Octree::CalculateMasses()
{
	if (starsNumber == 1)
	{
		return massData;
	}

	double massSum = 0.0;

	Vect3D positionSum = { 0 };

	for (int i = 0; i < 8; i++)
	{
		if (nodes[i] != nullptr)
		{
			MassData nodeMassData = nodes[i]->CalculateMasses();

			massSum += nodeMassData.mass;
			positionSum = positionSum + nodeMassData.position * nodeMassData.mass;
		}
	}

	MassData newMassData = { 0 };

	newMassData.mass = massSum;
	newMassData.position = positionSum / massSum;

	massData = newMassData;

	return newMassData;
}

Vect3D Octree::CalculateForceOnStar(Star* star)
{
	Vect3D force = { 0 };

	if (id == star->GetId())
	{
		return force;
	}

	Vect3D starPosition = star->GetPosition();

	double distance = Distance(starPosition, massData.position);

	if (distance < 10.0)
	{
		distance = 10.0;
	}

	if (starsNumber == 1 || box.size / distance < 1.0)
	{
		force = (massData.position - starPosition) * (star->GetMass() * massData.mass * GRAVITY) / pow(distance, 3);
	}
	else
	{
		for (int i = 0; i < 8; i++)
		{
			if (nodes[i] != nullptr)
			{
				force = force + nodes[i]->CalculateForceOnStar(star);
			}
		}
	}

	star->SetForce(force);

	return force;
}

Octree** Octree::GetNodes()
{
	return nodes;
}

int Octree::GetId()
{
	return id;
}

void Octree::SetId(int _id)
{
	id = _id;
}

int Octree::GetStarsNumber()
{
	return starsNumber;
}

void Octree::SetStarsNumber(int _starsNumber)
{
	starsNumber = _starsNumber;
}

Box Octree::GetBox()
{
	return box;
}

void Octree::SetBox(Box _box)
{
	box = _box;
}

MassData Octree::GetMassData()
{
	return massData;
}

void Octree::SetMassData(MassData _massData)
{
	massData = _massData;
}

Octree* CreateOctree(std::vector<Star*>& stars)
{
	Vect3D firstPos = stars[0]->GetPosition();

	Vect3D min = firstPos;
	Vect3D max = firstPos;

	for (int i = 1; i < stars.size(); i++)
	{
		Vect3D pos = stars[i]->GetPosition();

		if (pos.x < min.x) min.x = pos.x;
		if (pos.y < min.y) min.y = pos.y;
		if (pos.z < min.z) min.z = pos.z;

		if (pos.x > max.x) max.x = pos.x;
		if (pos.y > max.y) max.y = pos.y;
		if (pos.z > max.z) max.z = pos.z;
	}

	min.x -= 1.0;
	min.y -= 1.0;
	min.z -= 1.0;

	max.x += 1.0;
	max.y += 1.0;
	max.z += 1.0;

	double size = max.x - min.x;

	if (max.y - min.y > size)
	{
		size = max.y - min.y;
	}

	if (max.z - min.z > size)
	{
		size = max.z - min.z;
	}

	Octree* octree = new Octree;

	octree->SetBox({ min, size });

	for (int i = 0; i < stars.size(); i++)
	{
		octree->InsertStar(stars, stars[i]);
	}

	octree->DeleteEmptyLeaves();

	return octree;
}

void FreeOctree(Octree* octree)
{
	if (octree->GetStarsNumber() > 1)
	{
		Octree** nodes = octree->GetNodes();

		for (int i = 0; i < 8; i++)
		{
			if (nodes[i] != nullptr)
			{
				FreeOctree(nodes[i]);
			}
		}
	}

	delete octree;
}

bool IsInNode(Vect3D starPos, Box box)
{
	return starPos.x >= box.pos.x && starPos.x < box.pos.x + box.size &&
		starPos.y >= box.pos.y && starPos.y < box.pos.y + box.size &&
		starPos.z >= box.pos.z && starPos.z < box.pos.z + box.size;
}

void CalculateForces(std::vector<Star*>& stars, Octree* octree)
{
	for (int i = 0; i < stars.size(); i++)
	{
		octree->CalculateForceOnStar(stars[i]);
	}
}