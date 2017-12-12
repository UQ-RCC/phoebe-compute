#pragma once

#include <vtkPolyData.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkPolyDataNormals.h>
#include <vtkDecimatePro.h>
#include <vtkCleanPolyData.h>
#include <vtkInformation.h>

class SmoothPolyData
{
private:	
	vtkPolyDataNormals * normals;
	vtkSmoothPolyDataFilter * smooth;
	vtkCleanPolyData * clean;
	vtkDecimatePro * decimate;
public:
	SmoothPolyData();
	~SmoothPolyData();
	vtkPolyData * getPolyData(vtkPolyData * pd);
};
