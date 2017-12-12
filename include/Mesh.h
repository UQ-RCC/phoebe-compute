#pragma once
#include <string>
#include "Frame.h"

using namespace std;

class Mesh
{
public:
	int frameId;	
	string fileName;
	int objectCount = 0;
	int cellCount = 0;
	int indexCount = 0;
	double segISO = 0.0;
	string processCode;
	string meshCode;	
	string status;	
	Mesh();
	Mesh(Frame * const frame);
	void debugPrint();
};