#include <iostream>
#include <vtkVersion.h>
#include <vtkDirectory.h>
#include <vtkImageHistogram.h>
#include <vtkImageData.h>
#include <vector>
#include <string>
#include <libpq-fe.h>
#include <cstdlib>


#include <vtkImageReader2.h>
#include <boost/thread.hpp>
#include <vtkPolyDataWriter.h>

#include "PolyMaker.h"
#include "QuickView.h"
#include "MeshMakerIO.h"
#include "MeshLoader.h"
#include "Frame.h"
#include "MeshMakerDB.h"
#include "Mesh.h"
#include "CreatePolyData.h"
#include "SmoothPolyData.h"
#include "Console.h"

#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/timer/timer.hpp>
#include <boost/chrono.hpp>
#include <boost/program_options.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp> 

#include <json.hpp>

using namespace std;
using json = nlohmann::json;
namespace po = boost::program_options;
bool debugMode = false;

boost::uuids::random_generator generator;

void viewFile(const Frame df, string);
vector<string> getFileNames(string);
void processMesh(MeshMakerDB * db, int threadNum, const string rootDir, const bool debug, const bool vtkOutput, const int maxProc);
void savePolyData(const string rootDir, Mesh * mesh, vtkPolyData * polyData);
void saveMeshData(const string rootDir, Mesh * mesh, vtkPolyData * polyData);

void savePolyData(const string rootDir, Mesh * mesh, vtkPolyData * polyData)
{	
	boost::filesystem::path boostDir(FastViewIO::GetPath(rootDir, mesh->fileName));
	boost::filesystem::create_directories(boostDir);
	vtkPolyDataWriter * writer = vtkPolyDataWriter::New();
	string fileName = FastViewIO::GetFilePath(rootDir, mesh->fileName);
	fileName.append(".vtk");
	writer->SetFileName(fileName.c_str());
	writer->SetInputData(polyData);
	writer->SetFileTypeToBinary();
	writer->Write();
	writer->Delete();
}

void saveMeshData(const string rootDir, Mesh * mesh, vtkPolyData * polyData)
{	
	boost::filesystem::path boostDir(FastViewIO::GetPath(rootDir, mesh->fileName));
	bool boostResult = boost::filesystem::create_directories(boostDir);	
	FastViewIO::SavePolyData(FastViewIO::GetFilePath(rootDir, mesh->fileName), polyData);
}

unsigned long getThreadId()
{
	std::string threadId = boost::lexical_cast<std::string>(boost::this_thread::get_id());
	unsigned long threadNumber = 0;
	sscanf(threadId.c_str(), "%lx", &threadNumber);
	return threadNumber;
}

void processMesh(MeshMakerDB * db, int threadNum, const string rootDir, const bool debug, const bool vtkOutput, const int maxProc)
{
	CreatePolyData cp;
	SmoothPolyData sp;
	json logJ;
	
	unsigned u = boost::thread::hardware_concurrency();
	logJ["cores"] = u;

	if (getenv("PBS_QUEUE") != NULL)
	{		
		logJ["pbs"]["queue"] = getenv("PBS_QUEUE");
	}

	if (getenv("PBS_JOBID") != NULL)
	{
		logJ["pbs"]["jobID"] = getenv("PBS_JOBID");
	}

	if (getenv("PBS_ARRAYID") != NULL)
	{
		logJ["pbs"]["arrayID"] = getenv("PBS_ARRAYID");
	}

	logJ["thread"]["of"] = maxProc;
	logJ["thread"]["number"] = threadNum + 1;
	
	Frame * const frame = new Frame();

	if (Console::interactive)
	{
		Console::threadUpdate(threadNum, "init", NULL);
	}
	
	while (true)
	{		
		db->NextFrame(frame);
		while (frame->exists)
		{			
			boost::timer::cpu_timer t;
			int logID = db->StartLog();
			Mesh mesh(frame);			
			const string fileName = FastViewIO::GetFilePath(rootDir, frame->sourceFilename);
			if (debug)
			{
				cout << frame->sourceFilename << endl;
			}
			if (!boost::filesystem::exists(fileName))
			{				
				mesh.status = "error file not found";	
			}
			else
			{
				double nanoToSec = pow(10, -9);
				json meshLog(logJ);
				vtkPolyData * polyData = cp.getPolyData(rootDir, *frame, frame->segValue, 1000, meshLog);
				if (Console::interactive)
				{
					Console::threadUpdate(threadNum, "loaded", frame);
				}
				boost::timer::cpu_timer spd;
				vtkPolyData * smoothPolyData = sp.getPolyData(polyData);
				spd.stop();
				meshLog["vtk"]["smooth"]["system"] = spd.elapsed().system * nanoToSec;
				meshLog["vtk"]["smooth"]["usr"] = spd.elapsed().user * nanoToSec;
				meshLog["vtk"]["smooth"]["wall"] = spd.elapsed().wall * nanoToSec;
				mesh.cellCount = smoothPolyData->GetNumberOfCells();
				mesh.indexCount = 0;
				mesh.objectCount = cp.objectCount;
				mesh.segISO = frame->segValue;
				spd.start();
				saveMeshData(rootDir, &mesh, smoothPolyData);
				if (Console::interactive)
				{
					Console::threadUpdate(threadNum, "saved", frame);					
				}
				spd.stop();
				meshLog["vtk"]["save"]["system"] = spd.elapsed().system * nanoToSec;
				meshLog["vtk"]["save"]["usr"] = spd.elapsed().user * nanoToSec;
				meshLog["vtk"]["save"]["wall"] = spd.elapsed().wall * nanoToSec;
				mesh.status = "complete";
				int segID = db->InsertMesh(mesh);
				t.stop();
				meshLog["totalTime"]["system"] = t.elapsed().system * nanoToSec;
				meshLog["totalTime"]["usr"] = t.elapsed().user * nanoToSec;
				meshLog["totalTime"]["wall"] = t.elapsed().wall * nanoToSec;
				db->EndLog(logID, "segmentaion", segID, meshLog.dump());
			}
			if (Console::interactive)
			{
				Console::threadUpdate(threadNum, "fetch", frame);
			}
			db->NextFrame(frame);
		}
		if (Console::interactive)
		{
			Console::threadUpdate(threadNum, "idle", NULL);
		}
		boost::this_thread::sleep_for(boost::chrono::milliseconds(1000));
	}
	delete frame;
}

int main(int argc, char ** argv)
{	
	
	po::options_description desc("Allowed options");
	desc.add_options()
		("help,h", "this help message")
		("root,r", po::value<string>(), "root directory")		
		("processors,n", po::value<int>(), "maximum processors")
		("vtk-poly,v","generate vtk mesh files")
		("interactive,i", "interactive mode")
		("debug,d","debug mode");
	;
	
	po::variables_map vm;
	try
	{
		po::store(po::parse_command_line(argc, argv, desc), vm);
	}
	catch (po::error e)
	{
		cout << desc << endl;
		#ifdef WINDOWS
			while (true) {};
		#endif
		exit(1);
	}
	po::notify(vm);

	if (vm.count("help")) {
		cout << desc << endl;
		exit(0);
	}

	if (vm.count("interactive"))
	{
		Console::interactive = true;
		Console::println(1, "Console active");		
	}

	if (vm.count("debug"))
	{
		debugMode = true;
	}

	string root_directory = boost::filesystem::current_path().string();
	if (vm.count("root"))
	{
		root_directory = vm["root"].as<string>();
	}
	cout << "output directory: " << root_directory << endl;

	unsigned numberProc = boost::thread::hardware_concurrency();
	if (vm.count("processors"))
	{
		if (numberProc > vm["processors"].as<int>())
		{
			numberProc = vm["processors"].as<int>();
		}
		if (numberProc < 1)
		{
			numberProc = 1;
		}
	}

	cout << "processors: " << numberProc << endl;

	vtkVersion * vtkI = vtkVersion::New();
	cout << "vtk version: " << vtkI->GetVTKVersion() << endl;	
	MeshMakerDB * db = new MeshMakerDB();
	
	bool debug = false;
	if (vm.count("debug"))
	{
		cout << "debug mode" << endl;		
		debug = true;		
	}

	bool vtkOutput = false;
	if (vm.count("vtk-poly"))
	{
		cout << "generate vtk files" << endl;
		vtkOutput = true;
	}
			
	vector<boost::thread> threads;
	for (int i = 0; i < numberProc; i++)
	{
		threads.push_back(boost::thread(processMesh, db, i, root_directory, debug, vtkOutput, numberProc));
	}

	for (int i = 0; i < threads.size(); i++)
	{
		threads[i].join();
	}
	
#ifdef WINDOWS
	while (true) {};
#endif
	
	exit(0);

}

void viewFile(const Frame df, string inputDir)
{
	PolyMaker polyMaker(inputDir);
	vtkPolyData * polyData =  polyMaker.SetFiles(df, "");
	cout << "number points: " << polyData->GetNumberOfPoints() << endl;
	FastViewIO io;
	io.PrintMesh(polyData);
}

vector<string> getFileNames(string dir)
{
	vector<string> fileList;
	vtkDirectory * directory = vtkDirectory::New();
	directory->Open(dir.c_str());
	for (int i = 0; i < directory->GetNumberOfFiles(); i++)
	{
		string f = dir;
		f.append(directory->GetFile(i));
		if (!directory->FileIsDirectory(f.c_str()))
		{
			fileList.push_back(f);			
		}
	}
	return fileList;
}