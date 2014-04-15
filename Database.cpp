#include "Database.h"
#include "astrapmsg.h"
#include <syslog.h>

Database::Database(DatabaseType type)
{
	m_type = type;
	if (m_type == yd_server)
	{
#ifdef __YD_INTERFACE__
		m_yd_conn = new YdDatabase();
#endif
	}
	else
	{
		m_ora_conn = new ProcDatabase();

	}
}

Database::~Database()
{
	if (m_type == yd_server)
	{
#ifdef __YD_INTERFACE__
		if(m_yd_conn != NULL)
			delete m_yd_conn;
		m_yd_conn = NULL;
#endif
	}
	else
	{
		if(m_ora_conn != NULL)
			delete m_ora_conn;
		m_ora_conn = NULL;
	}
}

bool Database::InitDatabase(DatabaseType type)
{
	if (type == yd_server)
	{
#ifdef __YD_INTERFACE__
		YdDatabase::InitDatabase();
#endif
	}
	else
	{
		ProcDatabase::InitDatabase();
	}
}

bool Database::Open(char* db_name, char* user, char* pass)
{
	if (m_type == yd_server)
	{
#ifdef __YD_INTERFACE__
		return m_yd_conn->Open();
#endif
	}
	else
	{
		return m_ora_conn->Open(db_name, user, pass);
	}
}

bool Database::ExecUpdate(char* sql)
{
	if (m_type == yd_server)
	{
#ifdef __YD_INTERFACE__
		return m_yd_conn->ExecUpdate(sql);
#endif
	}
	else
	{
		return m_ora_conn->ExecUpdate(sql);
	}
}

bool Database::Begin()
{
	if (m_type == yd_server)
	{
#ifdef __YD_INTERFACE__
		return m_yd_conn->Begin();
#endif
	}
	else
	{
		return m_ora_conn->Begin();
	}
}

bool Database::Rollback()
{
	if (m_type == yd_server)
	{
#ifdef __YD_INTERFACE__
		return m_yd_conn->Rollback();
#endif
	}
	else
	{
		return m_ora_conn->Rollback();
	}
}

bool Database::Commit()
{
	if (m_type == yd_server)
	{
#ifdef __YD_INTERFACE__
		return m_yd_conn->Commit();
#endif
	}
	else
	{
		return m_ora_conn->Commit();
	}
}

bool Database::Close()
{
	if (m_type == yd_server)
	{
#ifdef __YD_INTERFACE__
		return m_yd_conn->Close();
#endif
	}
	else
	{
		return m_ora_conn->Close();
	}
}
