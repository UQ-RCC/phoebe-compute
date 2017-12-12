#pragma once

#include <libpq-fe.h>
#include <deque>
#include "Frame.h"
#include "Mesh.h"
#include <boost/asio.hpp>
#include <boost/thread.hpp>

class MeshMakerDB
{
private:
	const char * const conStr = "host=203.101.226.113 user=phoebeadmin dbname=phoebe password=password";	
	bool checkStmt(PGresult * result, PGconn * conn);
	PGconn * meshConn;
	PGconn * frameConn;	
	boost::mutex meshConnMtx;
	boost::mutex frameConnMtx;
	
public:
	MeshMakerDB();
	~MeshMakerDB();
	void NextFrame(Frame * const frame);
	int InsertMesh(const Mesh mesh);
	int StartLog();
	void EndLog(int logID, const string& type, int foreignID, const string& message);	
};