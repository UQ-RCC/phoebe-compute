#include "MeshMakerDB.h"

MeshMakerDB::MeshMakerDB()
{
	meshConn = PQconnectdb(conStr);
	frameConn = PQconnectdb(conStr);
}

MeshMakerDB::~MeshMakerDB()
{
	PQfinish(meshConn);
	PQfinish(frameConn);
}

class SqlBuilder
{
	
public:

	SqlBuilder(const string& function) : function(function) {}

	SqlBuilder& operator<<(const string& param)
	{	
		length.push_back(param.length());
		format.push_back(0);
		parameter.push_back(param.c_str());
		parameterDef.push_back(string().append("$").append(std::to_string(parameter.size())));
		return *this;
	}

	SqlBuilder& operator<<(const int param)
	{		
		length.push_back(sizeof(int));
		format.push_back(1);
		intKeeper.push_back(htonl(param));
		parameter.push_back((char *) &intKeeper.back());
		parameterDef.push_back(string().append("$").append(std::to_string(parameter.size())).append("::integer"));
		return *this;
	}

	string getSQL()
	{		
		string sql = "select * from ";
		sql.append(function).append("(");
		for (int i = 0; i < NumParameters() - 1; i++)
		{
			sql.append(parameterDef[i]);
			sql.append(", ");
		}
		if (NumParameters() > 0)
		{
			sql.append(parameterDef[NumParameters() - 1]);
		}
		sql.append(")");
		return sql;
	}

	int * GetParamLength()
	{
		return NumParameters() ? length.data() : NULL;		
	}

	int * GetParamFormat()
	{
		return NumParameters() ? format.data() : NULL;
	}

	const char ** getParameter()
	{
		return NumParameters() ? parameter.data() : NULL;
	}

	int NumParameters()
	{
		return parameter.size();
	}

	PGresult * execute(PGconn * meshConn)
	{
		return PQexecParams(meshConn, getSQL().c_str(), NumParameters(), NULL, getParameter(), GetParamLength(), GetParamFormat(), 0);
	}

	//This is how doubles are done (for future ref)	
	void to_nbo(const double in, double * out)
	{
		uint64_t * i = (uint64_t *)&in;
		uint32_t * r = (uint32_t *)out;

		/* convert input to network byte order */
		r[0] = htonl((uint32_t)((*i) >> 32));
		r[1] = htonl((uint32_t)*i);
	}

private:
	string function;
	vector<int> length;
	vector<int> format;
	vector<const char *> parameter;
	vector<string> parameterDef;
	deque<int> intKeeper;
};

void MeshMakerDB::NextFrame(Frame * const frame)
{
	boost::unique_lock<boost::mutex> lock(frameConnMtx);
	frame->exists = false;
	SqlBuilder sqb("neo_next_segmentation");
	PGresult * frameRS = sqb.execute(frameConn);
	if (checkStmt(frameRS, frameConn))
	{
		if (!PQgetisnull(frameRS, 0, 0))
		{
			frame->frameID = atoi(PQgetvalue(frameRS, 0, 0));
			frame->frameNumber = atoi(PQgetvalue(frameRS, 0, 1));
			frame->sourceFilename.assign(PQgetvalue(frameRS, 0, 2));
			frame->directory.assign(PQgetvalue(frameRS, 0, 3));
			frame->channelNumber = atoi(PQgetvalue(frameRS, 0, 4));
			frame->destinationFilename.assign(PQgetvalue(frameRS, 0, 5));
			frame->segValue = atof(PQgetvalue(frameRS, 0, 6));
			frame->width = atoi(PQgetvalue(frameRS, 0, 7));
			frame->height = atoi(PQgetvalue(frameRS, 0, 8));
			frame->depth = atoi(PQgetvalue(frameRS, 0, 9));
			frame->currentFrame = atoi(PQgetvalue(frameRS, 0, 10));
			frame->userName.assign(PQgetvalue(frameRS, 0, 11));
			frame->processId = atoi(PQgetvalue(frameRS, 0, 12));
			frame->queuedCount = atoi(PQgetvalue(frameRS, 0, 13));
			frame->exists = true;
			PQclear(frameRS);
		}
	}
}

int MeshMakerDB::InsertMesh(const Mesh mesh)
{
	boost::lock_guard<boost::mutex> lock(meshConnMtx);	
	SqlBuilder sqb("insert_segmentation");
	sqb << mesh.frameId << mesh.objectCount << mesh.cellCount << mesh.status;
	{		
		if (!checkStmt(sqb.execute(meshConn), meshConn))
		{
			printf("Insert mesh failed with code %s\n", mesh.processCode.c_str());				
		}
	}
	return mesh.frameId;
}

int MeshMakerDB::StartLog()
{
	SqlBuilder sqb("start_log");
	boost::unique_lock<boost::mutex> lock(meshConnMtx);	
	PGresult * logRS = sqb.execute(meshConn);
	if (checkStmt(logRS, meshConn))
	{
		if (!PQgetisnull(logRS, 0, 0))
		{
			return atoi(PQgetvalue(logRS, 0, 0));
		}
	}
	else
	{
		return 0;
	}
}

void MeshMakerDB::EndLog(int logID, const string & type, int foreignID, const string & message)
{
	SqlBuilder sqb("end_log");
	sqb << logID << type << foreignID << message;	
	boost::unique_lock<boost::mutex> lock(meshConnMtx);
	{		
		if (!checkStmt(sqb.execute(meshConn), meshConn))
		{
			printf("insert into end log failed\n");
		}
	}
}

bool MeshMakerDB::checkStmt(PGresult * result, PGconn * conn)
{
	if (PQresultStatus(result) != PGRES_TUPLES_OK)
	{
		fprintf(stderr, "SLQ error: %s\n", PQerrorMessage(conn));		
		fprintf(stderr, "State: %s\n", PQresultErrorField(result, PG_DIAG_SQLSTATE));
		fprintf(stderr, "Primary: %s\n", PQresultErrorField(result, PG_DIAG_MESSAGE_PRIMARY));
		fprintf(stderr, "Detail: %s\n", PQresultErrorField(result, PG_DIAG_MESSAGE_DETAIL));
		fprintf(stderr, "Pos: %s\n", PQresultErrorField(result, PG_DIAG_STATEMENT_POSITION));
		fprintf(stderr, "Context: %s\n", PQresultErrorField(result, PG_DIAG_CONTEXT));
		PQclear(result);
		return false;
	}
	return true;
}





