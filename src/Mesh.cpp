#include "Mesh.h"

Mesh::Mesh()
{
}

Mesh::Mesh(Frame * const frame)
{
	frameId = frame->frameID;	
	fileName = frame->destinationFilename;
	segISO = frame->segValue;
}

void Mesh::debugPrint()
{
	printf("id: %d\n", frameId);	
	printf("file: %s\n", fileName.c_str());
	printf("obs: %d\n", objectCount);
	printf("cells: %d\n", cellCount);
	printf("indices: %d\n", indexCount);
	printf("seg: %f\n", segISO);
	printf("code: %s\n", meshCode.c_str());
}

