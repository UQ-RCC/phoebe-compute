#pragma once

#include "fmt/format.h"
#include "fmt/printf.h"
#include <boost/thread.hpp>
#include "Frame.h"
#include <map>
#include <vector>

namespace Console
{
	using string = std::string;
	using frame_map = std::map<int, const Frame *>;
	using opp_map = std::map<int, string>;
	void println(int threadNum, const string & s);
	void printFrame(int threadNum, const string & operation, const Frame * const frame);
	void threadUpdate(int threadNum, const string & operation, const Frame * const frame);
	extern bool interactive;
	extern frame_map frameMap;
	extern opp_map oppMap;	
};
