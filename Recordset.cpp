#include "Recordset.h"
#include "Database.h"
#include "astrapmsg.h"
#include <string.h>

#ifdef __YD_INTERFACE__
#include "YD_Query.h"
#endif

Recordset::Recordset(Database* pdatabase)
{
	m_pdatabase = pdatabase;
	memset(&m_proc_set, 0, sizeof(Proc_Result));
}

Recordset::~Recordset()
{
	//Close();
}

bool Recordset::Open(const char* sql)
{
	if (m_pdatabase->m_type == yd_server)
	{
#ifdef __YD_INTERFACE__
		try
		{
			m_pdatabase->m_yd_conn->m_yd_conn->nExec(sql, 0, 0, 0);
			return true;
		}
		catch(Exception602 & ex)
		{
			trapmsg_normal_fmt("执行查询语句失败！errno[%d] errmsg[%s]", ex.getError(), ex.getMessage());
			return false;
		}
#endif
	}
	else
	{
		return exec_query(m_pdatabase->m_ora_conn->m_dbIndex, sql, &m_proc_set) == 0;
	}
}

bool Recordset::Close()
{
	if (m_pdatabase->m_type == yd_server)
	{
#ifdef __YD_INTERFACE__
		m_pdatabase->m_yd_conn->m_yd_conn->close();
#endif
	}
	else
	{
		if (m_proc_set.m_pbuffer != NULL)
		{
			delete m_proc_set.m_pbuffer;
			m_proc_set.m_pbuffer = NULL;

			//trapmsg_normal_fmt("m_pbuffer freed, size = %d", m_proc_set.m_row_total_length * m_proc_set.m_buffer_total);

			memset(&m_proc_set, 0, sizeof(Proc_Result));
		}
	}
	return true;
}

bool Recordset::IsEOF()
{
	if(m_pdatabase->m_type == yd_server)
	{
#ifdef __YD_INTERFACE__
		try
		{
			return m_pdatabase->m_yd_conn->m_yd_conn->bIsEOF();
		}
		catch(Exception602 & ex)
		{
			trapmsg_normal_fmt("异地接口IsEOF调用失败！errno[%d] errmsg[%s]", ex.getError(), ex.getMessage());
			return true;
		}
#endif
	}
	else
	{
		return m_proc_set.m_set_index == m_proc_set.m_row_number;
	}
}

bool Recordset::Next()
{
	if(m_pdatabase->m_type == yd_server)
	{
#ifdef __YD_INTERFACE__
		try
		{
			m_pdatabase->m_yd_conn->m_yd_conn->nMoveNext();
		}
		catch(Exception602 & ex)
		{
			trapmsg_normal_fmt("移动结果集指针失败！errno[%d] errmsg[%s]", ex.getError(), ex.getMessage());
			return false;
		}
#endif 
	}
	else
	{
		m_proc_set.m_set_index++;
	}

	return true;
}

string Recordset::GetStringValue(int index)
{
	string temp;
	int i;
	if(m_pdatabase->m_type == yd_server)
	{
#ifdef __YD_INTERFACE__
		try
		{
			char tm[1024];
			char* p;
			p = tm;
			m_pdatabase->m_yd_conn->m_yd_conn->nGetFieldValueByIndex(index, p);
			temp = tm;
		}
		catch(Exception602 & ex)
		{
			return temp;
		}
#endif 
	}
	else
	{
		int offset_index = 0;
		index--;
		for (i = 0; i < index; i++)
		{
			offset_index +=m_proc_set.m_col_length[i];
		}
		offset_index +=m_proc_set.m_set_index * m_proc_set.m_row_total_length;

		temp.assign(m_proc_set.m_pbuffer + offset_index, m_proc_set.m_col_length[index]);
	}

	return temp;
}

int Recordset::GetIntValue(int index)
{
	int temp;
	if(m_pdatabase->m_type == yd_server)
	{
#ifdef __YD_INTERFACE__
		try
		{
			char tm[1024];
			char* p;
			p = tm;
			m_pdatabase->m_yd_conn->m_yd_conn->nGetFieldValueByIndex(index, p);
			temp = atoi(tm);
		}
		catch(Exception602 & ex)
		{
			return -1;
		}
#endif 
	}
	else
	{
		int offset_index = 0;
		index--;
		for (int i = 0; i < index; i++)
		{
			offset_index +=m_proc_set.m_col_length[i];
		}
		offset_index +=m_proc_set.m_set_index *m_proc_set.m_row_total_length;

		temp = *(int*)(m_proc_set.m_pbuffer + offset_index);
	}

	return temp;
}
