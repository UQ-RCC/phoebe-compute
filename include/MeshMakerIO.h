#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkDataArray.h>
#include <vtkCell.h>
#include "MeshLoader.h"

using namespace std;

class FastViewIO
{
public:
	FastViewIO();
	~FastViewIO();

	static void SavePolyData(const string fileName, vtkPolyData * polyData);
	static const string GetPath(const string root, const string fileName) {
		return root + "/" + fileName.substr(0, 2) + "/" + fileName.substr(2, 2);		
	}
	static const string GetFilePath(const string root, const string fileName) {
		return GetPath(root, fileName) + "/" + fileName;
	}
	void PrintMesh(MeshBuffer * meshBuffer) const;
	void PrintMesh(vtkPolyData * polydata) const;
};

