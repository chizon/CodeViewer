#ifndef __PROCDATABASE_H__
#define __PROCDATABASE_H__

#include "Proc_utils.h"

class ProcDatabase
{
public:
	bool       m_bAutoCommit;
	int        m_dbIndex;

public:
	ProcDatabase();
	virtual ~ProcDatabase();

	static bool InitDatabase();

	bool Open(char* db_name, char* user, char* pass);
	bool Close();

	bool ExecUpdate(char* sql);           // ��ִ��SQL�����updata, insert, delete������䣬ִ�к��Զ��ύ
	bool Begin();
	bool Rollback();
	bool Commit();
};

#endif
