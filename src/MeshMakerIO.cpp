#include "MeshMakerIO.h"

FastViewIO::FastViewIO()
{
}


FastViewIO::~FastViewIO()
{
}

void FastViewIO::SavePolyData(const string fileName, vtkPolyData * polyData)
{
	ofstream out(fileName.c_str(), ios::binary);
	if (out.good())
	{		
		MeshHeader header;
		header.numPoint = polyData->GetNumberOfPoints();
		header.numIndices = polyData->GetNumberOfCells() * 3;
		for (size_t i = 0; i < 6; i++)
		{
			header.bounds[i] = polyData->GetBounds()[i];
		}
		out.write((char *)&header, sizeof(header));

		float f[3];
		for (int i = 0; i < polyData->GetNumberOfPoints(); i++)
		{
			double * d = polyData->GetPoint(i);
			f[0] = (float) d[0];
			f[1] = (float) d[1];
			f[2] = (float) d[2];
			out.write((char *) &f, sizeof(float) * 3);
		}

		vtkDataArray * normals = polyData->GetPointData()->GetNormals();
		for (int i = 0; i < polyData->GetNumberOfPoints(); i++)
		{
			double * d = normals->GetTuple3(i);
			f[0] = (float) d[0];
			f[1] = (float) d[1];
			f[2] = (float) d[2];
			out.write((char *) &f, sizeof(float) * 3);
		}

		for (int i = 0; i < polyData->GetNumberOfCells(); i++)
		{
			vtkCell * cell = polyData->GetCell(i);
			for (int j = 0; j < 3; j++)
			{
				int index = (int)cell->GetPointId(j);
				out.write((char *) &index, sizeof(index));
			}
		}		
		out.close();
	}

}

void FastViewIO::PrintMesh(MeshBuffer * meshBuffer) const
{

	double * b = meshBuffer->header->bounds;

	cout << "Number of points: " << meshBuffer->header->numPoint
		<< ", indices: " << meshBuffer->header->numIndices << endl
		<< "bounds [" << *b++ << "-" << *b++ << ", " << *b++ << "-"
		<< *b++ << ", " << *b++ << "-" << *b++ << "]" << endl;

	float * p = meshBuffer->points;
	float * n = meshBuffer->normals;
	
	for (int x = 0; x < 10; x++)
	{		
		printf("(%f, %f, %f) (%f, %f, %f)\n", p[0], p[1], p[2], n[0], n[1], n[2]);
		p += 3;
		n += 3;
	}
	int * index = meshBuffer->indices;
	int x = 0;
	for (int i = 0; i < 20; i++)
	{	
		printf("(%d, %d, %d)", index[0], index[1], index[2]);
		if (++x % 5 == 0)
		{
			printf("\n");
		}
		else
		{
			printf(", ");
		}
		index += 3;
	}
	
}

void FastViewIO::PrintMesh(vtkPolyData * polyData) const
{
	double * b = polyData->GetBounds();

	cout << "Number of points: " << polyData->GetNumberOfPoints()
		<< ", indices: " << polyData->GetNumberOfPolys() * 3 << endl
		<< "bounds [" << *b++ << "-" << *b++ << ", " << *b++ << "-"
		<< *b++ << ", " << *b++ << "-" << *b++ << "]" << endl;

	vtkDataArray * normals = polyData->GetPointData()->GetNormals();
	for (int x = 0; x < 10; x++)
	{
		double * p = polyData->GetPoint(x);
		double * n = normals->GetTuple3(x);
		double lenSq = n[0] * n[0] + n[1] * n[1] + n[2] * n[2];
		printf("(%f, %f, %f) (%f, %f, %f) len^2 = %f\n", p[0], p[1], p[2], n[0], n[1], n[2], lenSq);
		p += 3;
		n += 3;
	}

	int x = 0;
	for (int i = 0; i < 20; i++)
	{
		vtkCell * cell = polyData->GetCell(i);
		int i0 = cell->GetPointId(0);
		int i1 = cell->GetPointId(1);
		int i2 = cell->GetPointId(2);
		printf("(%d, %d, %d)", i0, i1, i2);		
		if (++x % 5 == 0)
		{
			printf("\n");
		}			
		else
		{
			printf(", ");
		}
	}

}
