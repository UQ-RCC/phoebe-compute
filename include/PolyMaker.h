#pragma once

#include <vtkPolyData.h>
#include <vtkImageData.h>
#include <vtkImageReader.h>
#include <vtkFlyingEdges3D.h>
#include <vtkPolyDataConnectivityFilter.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkPolyDataNormals.h>
#include <vtkIdTypeArray.h>
#include <vtkTimerLog.h>
#include <vtkPointData.h>
#include <vtkDecimatePro.h>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>
#include <string.h>

#include "Frame.h"
#include "MeshMakerIO.h"

using namespace std;
const double segmentationValue = 20.0;

class PolyMaker
{
private:
	vtkImageReader * sourceImageReader;	
	vtkFlyingEdges3D * flyingEdges;
	vtkPolyDataConnectivityFilter * connectivity;
	vtkSmoothPolyDataFilter * smoother;
	vtkDecimatePro * decimator;
	vtkPolyDataNormals * normals;
	vtkTimerLog * timerLog;
	vtkIdTypeArray * regionID;
	double actinThreshold = 20.0;
	const string pixel_root;
	const string buffer_root;
	const string vtk_root;
	FastViewIO mmIO;
public:
	PolyMaker(const string p);
	~PolyMaker();
	vtkPolyData * SetFiles(Frame sf, string targetFileName);
	void writeFile(vtkPolyData * polyData, const Frame frame);
};


