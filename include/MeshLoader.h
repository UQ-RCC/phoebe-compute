#pragma once

#include <string>
#include <iostream>
#include <fstream>

struct MeshHeader
{
	int numPoint;
	int numIndices;
	double bounds[6];
};

class MeshBuffer
{
public:
	MeshHeader * header;
	float * points;
	float * normals;
	int * indices;
};

MeshBuffer * loadMesh(const std::string fileName);