#ifndef __DATABASE_H__
#define __DATABASE_H__

#define __YD_INTERFACE__

#ifdef __YD_INTERFACE__
#include "YdDatabase.h"
#endif

#include "ProcDatabase.h"
#include "Recordset.h"

enum DatabaseType
{
	yd_server,
	ora_server
};

class Database
{
public:
	int  m_type;

#ifdef __YD_INTERFACE__
	YdDatabase*    m_yd_conn;
#endif
	ProcDatabase*  m_ora_conn;

public:
	Database(DatabaseType type);
	virtual ~Database();

	static bool InitDatabase(DatabaseType type);

	bool Open(char* db_name, char* user, char* pass);
	bool Close();

	bool ExecUpdate(char* sql);           // ��ִ��SQL�����updata, insert, delete������䣬ִ�к��Զ��ύ
	bool Begin();
	bool Rollback();
	bool Commit();
 };

#endif
