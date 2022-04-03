#include "BarnesHut.h"

Octree* CreateOctree(std::vector<Star>& stars)
{
	Vect3D firstPos = stars[0].GetPosition();

	Vect3D min = firstPos;
	Vect3D max = firstPos;

	for (int i = 1; i < stars.size(); i++)
	{
		Vect3D pos = stars[i].GetPosition();

		if (pos.x < min.x) min.x = pos.x;
		if (pos.y < min.y) min.y = pos.y;
		if (pos.z < min.z) min.z = pos.z;

		if (pos.x > max.x) max.x = pos.x;
		if (pos.y > max.y) max.y = pos.y;
		if (pos.z > max.z) max.z = pos.z;
	}

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
		InsertStar(stars, stars[i], octree);
	}

	DeleteEmptyLeaves(octree);

	return octree;
}

void InsertStar(std::vector<Star>& stars, Star star, Octree* octree)
{
	int starsNumber = octree->GetStarsNumber();

	if (starsNumber > 1)
	{
		Octree** nodes = octree->GetNodes();

		for (int i = 0; i < 8; i++)
		{
			if (IsInNode(star.GetPosition(), nodes[i]->GetBox()))
			{
				InsertStar(stars, star, nodes[i]);

				break;
			}
		}
	}
	else if (starsNumber == 1)
	{
		Box box = octree->GetBox();

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

		bool existingStarInserted = false;
		bool starInserted = false;

		for (int i = 0; i < 8; i++)
		{
			Octree* node = new Octree;

			node->SetBox({ positions[i], newSize });
			
			MassData massData = octree->GetMassData();

			if (!existingStarInserted && IsInNode(massData.position, node->GetBox()))
			{
				node->SetMassData(massData);
				node->SetStarsNumber(1);

				octree->SetMassData({ 0 });

				existingStarInserted = true;
			}

			if (!starInserted && IsInNode(star.GetPosition(), node->GetBox()))
			{
				InsertStar(stars, star, node);

				starInserted = true;
			}

			octree->SetNode(i, node);
		}
	}
	else
	{
		octree->SetMassData({ star.GetMass(), star.GetPosition() });
	}

	octree->SetStarsNumber(starsNumber + 1);
}

void DeleteEmptyLeaves(Octree* octree)
{
	Octree** nodes = octree->GetNodes();

	for (int i = 0; i < 8; i++)
	{
		int starsNumber = nodes[i]->GetStarsNumber();

		if (starsNumber == 0)
		{
			delete nodes[i];
			nodes[i] = nullptr;
		}
		else if (starsNumber > 1)
		{
			DeleteEmptyLeaves(nodes[i]);
		}
	}
}

MassData CalculateMasses(Octree* octree)
{
	MassData massData = { 0 };

	if (octree->GetStarsNumber() == 1)
	{
		massData = octree->GetMassData();
	}
	else
	{
		Octree** nodes = octree->GetNodes();

		double massSum = 0.0;

		Vect3D positionSum = { 0 };

		for (int i = 0; i < 8; i++)
		{
			if (nodes[i] != nullptr)
			{
				MassData massData = CalculateMasses(nodes[i]);

				massSum += massData.mass;
				positionSum = positionSum + massData.position * massData.mass;
			}
		}

		massData.mass = massSum;
		massData.position = positionSum / massSum;
	}

	octree->SetMassData(massData);

	return massData;
}

void CalculateForces(std::vector<Star>& stars, Octree* octree)
{
	for (int i = 0; i < stars.size(); i++)
	{
		CalculateForceOnStar(&stars[i], octree);
	}
}

Vect3D CalculateForceOnStar(Star* star, Octree* node)
{
	Vect3D force = { 0 };

	Vect3D starPos = star->GetPosition();
	double starMass = star->GetMass();

	MassData massData = node->GetMassData();

	double distance = Distance(starPos, massData.position);

	if (node->GetStarsNumber() == 1)
	{
		force = Force(starPos, starMass, massData.position, massData.mass, distance);
	}
	else
	{
		if (node->GetBox().size / distance < 1.0)
		{
			force = Force(starPos, starMass, massData.position, massData.mass, distance);
		}
		else
		{
			Octree** nodes = node->GetNodes();

			for (int i = 0; i < 8; i++)
			{
				if (nodes[i] != nullptr)
				{
					force = force + CalculateForceOnStar(star, nodes[i]);
				}
			}
		}
	}

	star->SetForce(force);

	return force;
}

bool IsInNode(Vect3D starPos, Box box)
{
	return starPos.x >= box.pos.x && starPos.x < box.pos.x + box.size &&
		starPos.y >= box.pos.y && starPos.y < box.pos.y + box.size &&
		starPos.z >= box.pos.z && starPos.z < box.pos.z + box.size;
}

Vect3D Force(Vect3D pos, double mass, Vect3D targetPos, double targetMass, double distance)
{
	if (distance < 10.0)
	{
		distance = 10.0;
	}

	return (targetPos - pos) * (mass * targetMass * GRAVITY) / pow(distance, 3);
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

int Octree::GetStarsNumber()
{
	return starsNumber;
}

void Octree::SetStarsNumber(int _starsNumber)
{
	starsNumber = _starsNumber;
}

Octree** Octree::GetNodes()
{
	return nodes;
}

void Octree::SetNode(int index, Octree* node)
{
	nodes[index] = node;
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