#pragma once

#include <vtkImageData.h>
#include <vtkImageReader.h>
#include <vtkFlyingEdges3D.h>
#include <vtkPolyDataConnectivityFilter.h>
#include <vtkPolyData.h>
#include "Frame.h"
#include "MeshMakerIO.h"
#include <boost/timer/timer.hpp>
#include <string.h>
#include <json.hpp>

using namespace std;
using json = nlohmann::json;

class CreatePolyData
{
private:
	vtkImageReader * imageReader;
	vtkFlyingEdges3D * flyingEdges;
	vtkPolyDataConnectivityFilter * connectivity;	
	void appendLog(const boost::timer::cpu_timer &t, const string& operation, json &logJ);
	const double nanoToSec = pow(10, -9);
public:	
	int objectCount = 0;
	CreatePolyData();
	~CreatePolyData();
	vtkPolyData * getPolyData(const string& rootDir, const Frame& fr, const double sv, const double sl, json &logJ);
};
