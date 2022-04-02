#include "BarnesHut.h"

Octree* CreateOctree(std::vector<Star>& stars)
{
	Octree* octree = new Octree;
	
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

	octree->SetBox({ min, size });
	
	for (int i = 0; i < stars.size(); i++)
	{
		InsertStar(stars, stars[i], octree);
	}

	DeleteEmptyLeaves(octree);

	return octree;
}

void DeleteEmptyLeaves(Octree* octree)
{
	Octree** nodes = octree->GetNodes();

	for (int i = 0; i < 8; i++)
	{
		if (nodes[i] != nullptr)
		{
			if (nodes[i]->IsLeaf() && nodes[i]->GetStarsNumber() == 0)
			{
				FreeOctree(nodes[i]);
				nodes[i] = nullptr;
			}
			else
			{
				DeleteEmptyLeaves(nodes[i]);
			}
		}
	}
}

bool IsInNode(Vect3D starPos, Box box)
{
	return starPos.x >= box.pos.x && starPos.x < box.pos.x + box.size &&
		starPos.y >= box.pos.y && starPos.y < box.pos.y + box.size &&
		starPos.z >= box.pos.z && starPos.z < box.pos.z + box.size;
}

void CreateNodes(Octree* octree, Box box, double newSize)
{
	Vect3D positions[8] =
	{
		{ box.pos.x, box.pos.y, box.pos.z },
		{ box.pos.x + newSize, box.pos.y, box.pos.z },
		{ box.pos.x, box.pos.y + newSize, box.pos.z },
		{ box.pos.x, box.pos.y, box.pos.z + newSize },
		{ box.pos.x + newSize, box.pos.y + newSize, box.pos.z },
		{ box.pos.x + newSize, box.pos.y, box.pos.z + newSize },
		{ box.pos.x, box.pos.y + newSize, box.pos.z + newSize },
		{ box.pos.x + newSize, box.pos.y + newSize, box.pos.z + newSize }
	};

	for (int i = 0; i < 8; i++)
	{
		Octree* node = new Octree;
		node->SetBox({ positions[i], newSize });

		octree->SetNode(i, node);
	}

	octree->SetLeaf(false);
}

void InsertStar(std::vector<Star>& stars, Star star, Octree* octree)
{
	Box box = octree->GetBox();

	double newSize = box.size / 2.0;

	int starsNumber = octree->GetStarsNumber();

	octree->SetStarsNumber(starsNumber + 1);

	if (starsNumber > 1)
	{
		Octree** nodes = octree->GetNodes();

		for (int i = 0; i < 8; i++)
		{
			if (nodes[i] != nullptr)
			{
				if (IsInNode(star.GetPosition(), nodes[i]->GetBox()))
				{
					InsertStar(stars, star, nodes[i]);
				}
			}
		}
	}
	else if (starsNumber == 1)
	{
		if (octree->IsLeaf())
		{
			CreateNodes(octree, box, newSize);
		}

		Octree** nodes = octree->GetNodes();

		for (int i = 0; i < 8; i++)
		{
			if (nodes[i] != nullptr)
			{
				if (IsInNode(octree->GetPosition(), nodes[i]->GetBox()))
				{
					nodes[i]->SetPosition(octree->GetPosition());
					nodes[i]->SetMass(octree->GetMass());
					nodes[i]->SetStarsNumber(1);

					octree->SetPosition({ 0 });
					octree->SetMass(0.0);

					break;
				}
			}
		}

		for (int i = 0; i < 8; i++)
		{
			if (nodes[i] != nullptr)
			{
				if (IsInNode(star.GetPosition(), nodes[i]->GetBox()))
				{
					InsertStar(stars, star, nodes[i]);

					break;
				}
			}
		}
	}
	else
	{
		octree->SetPosition(star.GetPosition());
		octree->SetMass(star.GetMass());
	}
}

void FreeOctree(Octree* octree)
{
	if (!octree->IsLeaf())
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

MassData CalculateMasses(Octree* octree)
{
	MassData massData = { 0 };

	if (octree->GetStarsNumber() == 1)
	{
		massData.mass = octree->GetMass();
		massData.centerOfMass = octree->GetPosition();
	}
	else
	{
		Octree** nodes = octree->GetNodes();

		double massSum = 0.0;

		Vect3D sum = { 0 };

		for (int i = 0; i < 8; i++)
		{
			if (nodes[i] != nullptr)
			{
				MassData massData = CalculateMasses(nodes[i]);

				sum = sum + massData.centerOfMass * massData.mass;
				massSum += massData.mass;
			}
		}

		massData.mass = massSum;
		massData.centerOfMass = sum / massSum;
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

Vect3D CalculerForce(Vect3D pos, double mass, Vect3D targetPos, double targetMass)
{
	double distance = Distance(pos, targetPos);

	if (distance < 10.0)
	{
		distance = 10.0;
	}

	return (targetPos - pos) * (mass * targetMass * GRAVITY) / pow(distance, 3);
}

Vect3D CalculateForceOnStar(Star* star, Octree* node)
{
	Vect3D starPos = star->GetPosition();
	double starMass = star->GetMass();

	Vect3D force = { 0 };

	if (node->GetStarsNumber() == 1)
	{
		force = CalculerForce(starPos, starMass, node->GetPosition(), node->GetMass());
	}
	else
	{
		MassData massData = node->GetMassData();

		if (node->GetBox().size / Distance(starPos, massData.centerOfMass) < 1.0)
		{
			force = CalculerForce(starPos, starMass, massData.centerOfMass, massData.mass);
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

int Octree::GetStarsNumber()
{
	return starsNumber;
}

void Octree::SetStarsNumber(int _starsNumber)
{
	starsNumber = _starsNumber;
}

Vect3D Octree::GetPosition()
{
	return position;
}

void Octree::SetPosition(Vect3D _position)
{
	position = _position;
}

double Octree::GetMass()
{
	return mass;
}

void Octree::SetMass(double _mass)
{
	mass = _mass;
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

bool Octree::IsLeaf()
{
	return isLeaf;
}

void Octree::SetLeaf(bool _isLeaf)
{
	isLeaf = _isLeaf;
}