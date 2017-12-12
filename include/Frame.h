#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>

using namespace std;
struct Frame
{
public:
	bool exists = false;
	int frameID;
	int frameNumber;
	double segValue;
	int width;
	int height;
	int depth;	
	string sourceFilename;	
	string destinationFilename;
	string operation;
	string directory;
	int channelNumber;
	int currentFrame;
	string userName;
	int processId;
	int queuedCount;
	bool operator == (const Frame &rhs) const {
		return
			(frameID == rhs.frameID) &&
			(frameNumber == rhs.frameNumber) &&
			(sourceFilename.compare(rhs.sourceFilename) == 0) &&
			(destinationFilename.compare(rhs.destinationFilename) == 0) &&
			(operation.compare(rhs.operation) == 0) &&
			(width == rhs.width) &&
			(height == rhs.height) &&
			(depth == rhs.depth) &&
			(exists == rhs.exists) &&
			(segValue == rhs.segValue) &&
			(directory.compare(rhs.directory) == 0) &&
			(channelNumber == rhs.channelNumber) &&
			(currentFrame == rhs.currentFrame) &&
			(processId == rhs.processId) &&
			(queuedCount == rhs.queuedCount) &&
			(userName.compare(rhs.userName) == 0);
	}
	bool operator != (const Frame &rhs) const { return !(*this == rhs); }	
};
