#include "MeshLoader.h"

using namespace std;

MeshBuffer * loadMesh(const std::string fileName)
{
	MeshBuffer * meshBuffer = new MeshBuffer();
	ifstream in(fileName.c_str(), ios::in | ios::binary | ios::ate);
	if (in.is_open())
	{
		streampos size = in.tellg();
		char * memblock = new char[size];
		in.seekg(0, ios::beg);
		in.read(memblock, size);
		in.close();
		meshBuffer->header = (MeshHeader *)memblock;
		int numPoints = meshBuffer->header->numPoint;		
		int pointBufferSize = numPoints * sizeof(float) * 3;
		meshBuffer->points = (float *)(memblock + sizeof(MeshHeader));
		meshBuffer->normals = (float *)(memblock + sizeof(MeshHeader) + pointBufferSize);		
		meshBuffer->indices = (int *)(memblock + sizeof(MeshHeader) + pointBufferSize * 2);
	}
	return meshBuffer;

}
