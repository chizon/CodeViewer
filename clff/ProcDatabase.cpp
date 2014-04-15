#include "ProcDatabase.h"

ProcDatabase::ProcDatabase()
{
	m_bAutoCommit = true;
	m_dbIndex = -1;
}

ProcDatabase::~ProcDatabase()
{
	Close();
}

bool ProcDatabase::InitDatabase()
{
	init_database();
}

bool ProcDatabase::Open(char* db_name, char* user, char* pass)
{
	int index = connect_db(db_name, user, pass);
	if ( index >= 0)
	{
		m_dbIndex = index;
		return true;
	}
	else
		return false;
}

bool ProcDatabase::ExecUpdate(char* sql)
{
	if (m_dbIndex >= 0)
		return exec_update(m_dbIndex, sql, m_bAutoCommit) >= 0;
	else
		return false;
}

bool ProcDatabase::Begin()
{
	m_bAutoCommit = false;

	return true;
}

bool ProcDatabase::Rollback()
{
	m_bAutoCommit = true;
	roll_back(m_dbIndex);

	return true;
}

bool ProcDatabase::Commit()
{
	m_bAutoCommit = true;
	commit(m_dbIndex);

	return true;
}

bool ProcDatabase::Close()
{
	m_bAutoCommit = true;
	closedb(m_dbIndex);

	return true;
}
