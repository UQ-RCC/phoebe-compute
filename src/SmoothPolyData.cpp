#include "SmoothPolyData.h"

SmoothPolyData::SmoothPolyData()		
	: decimate(vtkDecimatePro::New())
	, clean(vtkCleanPolyData::New())
	, normals(vtkPolyDataNormals::New())
	, smooth(vtkSmoothPolyDataFilter::New())
{	
	smooth->SetNumberOfIterations(10);
	smooth->SetRelaxationFactor(0.1);	
	smooth->BoundarySmoothingOn();

	decimate->SetInputConnection(smooth->GetOutputPort());
	decimate->SetTargetReduction(0.5);
	
	normals->SetInputConnection(decimate->GetOutputPort());
	normals->ConsistencyOn();
	normals->AutoOrientNormalsOn();
	normals->SplittingOff();	
}

SmoothPolyData::~SmoothPolyData()
{
	normals->Delete();
	smooth->Delete();
	decimate->Delete();	
}

vtkPolyData * SmoothPolyData::getPolyData(vtkPolyData * pd)
{
	smooth->SetInputData(pd);
	normals->Update();
	return normals->GetOutput();
}
