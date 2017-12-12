#include "QuickView.h"

QuickView::QuickView(vector<vtkPolyData *> pd) :	
	renderer(vtkRenderer::New()),
	renderWindow(vtkRenderWindow::New()),
	interactor(vtkRenderWindowInteractor::New())	
{	
	polyData = pd;
	for (int i = 0; i < pd.size(); i++)
	{
		
		vtkPolyDataMapper * m = vtkPolyDataMapper::New();
		m->SetInputData(pd[i]);		
		m->ScalarVisibilityOff();
		vtkActor * a = vtkActor::New();		
		a->SetMapper(m);
		a->GetProperty()->SetColor(0.4, 0.8, 0.4);
		if (i == 0)
		{
			a->VisibilityOn();
		}
		else
		{
			a->VisibilityOff();
		}
		m->Update();
		mapper.push_back(m);
		actor.push_back(a);
		renderer->AddActor(a);
	}

	renderer->SetBackground(0.3,0.3,0.8);
	renderWindow->SetSize(200, 200);
	renderWindow->AddRenderer(renderer);
	interactor->SetRenderWindow(renderWindow);	
	interactor->AddObserver(vtkCommand::KeyPressEvent, this);
	interactor->Start();
}

QuickView::~QuickView()
{
	for (int i = 0; i < polyData.size(); i++)
	{
		actor[i]->Delete();
		mapper[i]->Delete();
		polyData[i]->Delete();
	}
	//renderer->Delete();
	//renderWindow->Delete();
	//interactor->Delete();
}

void QuickView::Execute(vtkObject * caller, unsigned long eid , void * clientData)
{	
	vtkRenderWindowInteractor * iren = static_cast<vtkRenderWindowInteractor*>(caller);	
	if (*iren->GetKeySym() == 'n')
	{
		actor[currentFrame]->VisibilityOff();
		currentFrame++;
		if (currentFrame == polyData.size())
		{
			currentFrame = 0;
		}
		actor[currentFrame]->VisibilityOn();
		renderWindow->Render();
		int c = polyData[currentFrame]->GetNumberOfCells();
		printf("frame %d : %d\n", currentFrame, c);
	}
}