#include "CreatePolyData.h"

CreatePolyData::CreatePolyData()
	: imageReader(vtkImageReader::New())
	, flyingEdges(vtkFlyingEdges3D::New())
	, connectivity(vtkPolyDataConnectivityFilter::New())
{
	flyingEdges->SetInputConnection(imageReader->GetOutputPort());
	connectivity->SetExtractionModeToSpecifiedRegions();
	connectivity->SetInputConnection(flyingEdges->GetOutputPort());
}

CreatePolyData::~CreatePolyData()
{
	connectivity->Delete();
	flyingEdges->Delete();
	imageReader->Delete();
}

vtkPolyData * CreatePolyData::getPolyData(const string& rootDir, const Frame& fr, const double sv, const double sl, json &logJ)
{
	objectCount = 0;
	
	boost::timer::cpu_timer t;
	imageReader->SetFileName(FastViewIO::GetFilePath(rootDir, fr.sourceFilename).c_str());
	imageReader->SetDataExtent(0, fr.width - 1, 0, fr.height - 1, 0, fr.depth - 1);
	imageReader->SetDataScalarTypeToUnsignedChar();
	imageReader->SetFileDimensionality(3);
	imageReader->SetNumberOfScalarComponents(1);
	imageReader->SetDataSpacing(1, 1, 1);
	imageReader->SetDataByteOrderToBigEndian();
	imageReader->Update();
	t.stop();
	appendLog(t, "load", logJ);

	
	t.start();
	flyingEdges->SetValue(0, sv);
	flyingEdges->Update();
	t.stop();
	appendLog(t, "segmentation", logJ);
		
	t.start();
	connectivity->Update();
	vtkIdTypeArray * regionSize = connectivity->GetRegionSizes(); // Size in cell count
	connectivity->InitializeSpecifiedRegionList();	
	for (int i = 0; i < regionSize->GetSize(); i++)
	{
		if (regionSize->GetValue(i) >= sl)
		{
			objectCount++;
			connectivity->AddSpecifiedRegion(i);
		}
	}
	connectivity->Update();
	appendLog(t, "connectiviy", logJ);
			
	return connectivity->GetOutput();

}

void CreatePolyData::appendLog(const boost::timer::cpu_timer &t, const string & operation, json & logJ)
{
	logJ["vtk"][operation]["system"] = t.elapsed().system * nanoToSec;
	logJ["vtk"][operation]["usr"] = t.elapsed().user * nanoToSec;
	logJ["vtk"][operation]["wall"] = t.elapsed().wall * nanoToSec;
}
