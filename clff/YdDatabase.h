#ifndef __YDDATABASE_H__
#define __YDDATABASE_H__

#include "YD_Query.h"

#include <string>
using namespace std;

class YD_SServer;
class YD_Result;
class Recordset;
class YdDatabase
{
public:
	//static YD_SServer* g_yd_server;
	YD_SServer g_yd_server;

	YD_Result* m_yd_conn;
	bool       m_bAutoCommit;

public:
	YdDatabase();
	~YdDatabase();

	static bool InitDatabase();

	bool Open();
	bool Close();

	bool ExecUpdate(char* sql);           // 可执行SQL语句中updata, insert, delete更新语句，执行后自动提交
	bool Begin();
	bool Rollback();
	bool Commit();
};

#endif
