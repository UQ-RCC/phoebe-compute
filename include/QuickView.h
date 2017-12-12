#pragma once

#include <iostream>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkPolyData.h>
#include <vtkObject.h>
#include <vtkCallbackCommand.h>
#include <vtkCommand.h>
#include <vector>

using namespace std;
class QuickView : vtkCallbackCommand
{
private:
	vector<vtkPolyData *> polyData;
	vector<vtkActor *> actor;
	vector<vtkPolyDataMapper *> mapper;
	vtkRenderer * renderer;
	vtkRenderWindow * renderWindow;
	vtkRenderWindowInteractor * interactor;
	int currentFrame = 0;
public:	
	void Execute(vtkObject * caller, unsigned long eid, void * clientData);
	QuickView(vector<vtkPolyData *> polyData);
	~QuickView();
};

