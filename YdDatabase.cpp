#include "YdDatabase.h"
#include "Recordset.h"
#include "astrapmsg.h"

//YD_SServer* YdDatabase::g_yd_server = NULL;

bool YdDatabase::InitDatabase()
{
	//g_yd_server = new YD_SServer();
	//if (g_yd_server)	return true;
	//else				return false;

	return true;
}

YdDatabase::YdDatabase(void)
{
	m_bAutoCommit = true;
}

YdDatabase::~YdDatabase(void)
{
	//Close();
}

bool YdDatabase::Open()
{
	for(int i = 1; i <= 3; i++)
	{
		try
		{
			//m_yd_conn = g_yd_server->ResConnect("gjzx_clgl.x", 0);
			m_yd_conn = g_yd_server.ResConnect("gjzx_clgl.x", 0);
			if(m_yd_conn != NULL)
				break;
		}
		catch(Exception602 & ex)
		{
			trapmsg_normal_fmt("数据库连接失败！errno[%d] errmsg[%s]", ex.getError(), ex.getMessage());
			m_yd_conn = NULL;
		}
		sleep(1);
	}

	if(m_yd_conn == NULL)
		return false;
	else
		return true;
}

bool YdDatabase::ExecUpdate(char* sql)
{
	try
	{
		m_yd_conn->nExec(sql, 0, 0, 0);
	}
	catch(Exception602 & ex)
	{
		trapmsg_normal_fmt("执行更新语句失败！errno[%d] errmsg[%s]", ex.getError(), ex.getMessage());
		return false;
	}
	return true;
}

bool YdDatabase::Begin()
{
	m_bAutoCommit = false;
	try
	{
		m_yd_conn->nBegin(0);
	}
	catch(Exception602 & ex)
	{
		trapmsg_normal_fmt("开始一个新事务失败！errno[%d] errmsg[%s]", ex.getError(), ex.getMessage());
		return false;
	}
	return true;
}

bool YdDatabase::Rollback()
{
	m_bAutoCommit = true;
	try
	{
		m_yd_conn->nRollBack(0);
	}
	catch(Exception602 & ex)
	{
		trapmsg_normal_fmt("回滚事务失败！errno[%d] errmsg[%s]", ex.getError(), ex.getMessage());
		return false;
	}

	return true;
}

bool YdDatabase::Commit()
{
	m_bAutoCommit = true;
	try
	{
		m_yd_conn->nCommit(0);
	}
	catch(Exception602 & ex)
	{
		trapmsg_normal_fmt("向数据库提交信息失败！errno[%d] errmsg[%s]", ex.getError(), ex.getMessage());
		return false;
	}

	return true;
}
bool YdDatabase::Close()
{
	try	
	{
		if(m_yd_conn)
		{
			//m_yd_conn->close();
			delete m_yd_conn;
		}
		m_yd_conn = NULL;
	}
	catch(Exception602 & ex)
	{
		trapmsg_normal_fmt("关闭数据库连接失败！errno[%d] errmsg[%s]", ex.getError(), ex.getMessage());
		return false;
	}

	return true;
}

