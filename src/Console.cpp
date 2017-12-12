#include "Console.h"
using namespace Console;

static boost::mutex conMtx;

void Console::println(int threadNum, const string & s)
{
	fmt::printf("%02d: %s\n", threadNum, s.c_str());
}

void Console::printFrame(int threadNum, const string & operation, const Frame * const frame)
{	
	if (frame != NULL)
	{

		string current = frame->currentFrame == frame->frameNumber ? "*" : "";
		//string current = "";

		fmt::printf("%02d: %1s %6s %10s %06d %06d %04d %s %d\n", 
			threadNum, current.c_str(), operation.c_str(), frame->userName.c_str(), frame->currentFrame, frame->frameNumber, frame->frameID,
			frame->directory.c_str(), frame->queuedCount);
	}
	else
	{
		fmt::printf("%02d: %s\n", threadNum, operation.c_str());
	}
}

void Console::threadUpdate(int threadNum, const string & operation, const Frame * const frame)
{	
	boost::unique_lock<boost::mutex> lock(conMtx);	
	frameMap[threadNum] = frame;
	oppMap[threadNum] = operation;

	typedef std::function<bool(std::pair<int, const Frame *>, std::pair<int, const Frame *>)> Comparator;

	Comparator compFunctor = [](std::pair<int, const Frame *> elem1, std::pair<int, const Frame *> elem2)
	{
		int f1 = elem1.second == NULL ? -1 : elem1.second->frameNumber;
		int f2 = elem2.second == NULL ? -1 : elem2.second->frameNumber;
		return f1 > f2;
	};

	std::vector<std::pair<int, const Frame *>> frameSet(frameMap.begin(), frameMap.end());
	std::sort(frameSet.begin(), frameSet.end(), compFunctor);

	fmt::printf("\e[1;1H\e[2J");
	for (const auto& f : frameSet)
	{
		printFrame(f.first, oppMap[f.first], f.second);
	}
}

bool Console::interactive = false;
frame_map Console::frameMap;
opp_map Console::oppMap;
