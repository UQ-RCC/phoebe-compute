#include "PolyMaker.h"

PolyMaker::PolyMaker(const string p)
	: sourceImageReader(vtkImageReader::New())
	, flyingEdges(vtkFlyingEdges3D::New())
	, connectivity(vtkPolyDataConnectivityFilter::New())
	, smoother(vtkSmoothPolyDataFilter::New())
	, decimator(vtkDecimatePro::New())
	, normals(vtkPolyDataNormals::New())
	, timerLog(vtkTimerLog::New())
	, pixel_root(p + "/pixel")
	, buffer_root(p + "/buffer")
	, vtk_root(p + "/vtk")
{
	flyingEdges->SetValue(0, 5.0);
	flyingEdges->SetInputConnection(sourceImageReader->GetOutputPort());

	connectivity->SetExtractionModeToSpecifiedRegions();
	connectivity->SetInputConnection(flyingEdges->GetOutputPort());

	smoother->SetNumberOfIterations(20);
	smoother->SetRelaxationFactor(0.1);
	smoother->SetFeatureAngle(60.0);
	smoother->BoundarySmoothingOn();
	smoother->SetInputConnection(connectivity->GetOutputPort());

	decimator->SetTargetReduction(0.75);
	decimator->SetInputConnection(smoother->GetOutputPort());

	normals->AutoOrientNormalsOn();
	normals->ComputePointNormalsOn();
	normals->SplittingOff();
	normals->SetInputConnection(decimator->GetOutputPort());
	
}

PolyMaker::~PolyMaker()
{
}

vtkPolyData * PolyMaker::SetFiles(Frame frame, const string targetFileName)
{
	string fileName = pixel_root; // TODO More filename changes
	fileName += "/";		
	fileName += frame.sourceFilename;
	sourceImageReader->SetFileName(fileName.c_str());
	sourceImageReader->SetDataExtent(0, frame.width - 1, 0, frame.height - 1, 0, frame.depth - 1);
	sourceImageReader->SetDataScalarTypeToUnsignedChar();
	sourceImageReader->SetFileDimensionality(3);
	sourceImageReader->SetNumberOfScalarComponents(1);
	sourceImageReader->SetDataSpacing(1, 1, 1);
	sourceImageReader->SetDataByteOrderToBigEndian();

	connectivity->Update();
	vtkIdTypeArray * regionID = connectivity->GetRegionSizes();
	for (int i = 0; i < regionID->GetSize(); i++)
	{
		if (regionID->GetValue(i) > 1000)
		{
			connectivity->AddSpecifiedRegion(i);
		}
	}

	normals->Update();
	frame.operation = "F";

	//writeFile(normals->GetOutput(), frame);
	//normals->SetInputConnection(decimate->GetOutputPort());
	//normals->Update();
	//frame.operation = "D";
	//writeFile(decimate->GetOutput(), frame);

	return normals->GetOutput();

}

void PolyMaker::writeFile(vtkPolyData * polyData, const Frame frame)
{
	//TODO More changes to filename
	string filePrefix = buffer_root  + "/" + frame.sourceFilename + "_" + frame.operation;
	string fileName = filePrefix + "-.buf";
	boost::filesystem::path boostDir(buffer_root);	
	if (boost::filesystem::create_directories(boostDir))
	{
		printf("created a dir!!\n");
	}
	mmIO.SavePolyData(fileName.c_str(), polyData);

}
