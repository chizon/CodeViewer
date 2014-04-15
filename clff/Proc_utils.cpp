#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>
#include <getopt.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>
#include <limits.h>
#include <regex.h>
#include <ctype.h>
#include <syslog.h>
#include <sqlca.h>
#include <sqlda.h>
#include <oraca.h>
#include "Proc_utils.h"

EXEC ORACLE OPTION (char_map = string);
EXEC ORACLE OPTION (ORACA = YES);

#define MAX_ORA_CONNECTION_AMOUNT 40

sql_context ora_connections[MAX_ORA_CONNECTION_AMOUNT];
int         conn_status[MAX_ORA_CONNECTION_AMOUNT];
char        ora_tnsname[MAX_ORA_CONNECTION_AMOUNT][20];

#define MAX_ITEMS         40	/* 选择列表项的最大数量. */
#define MAX_VNAME_LEN     30	/* 选择列表项名字的最大长度 */
#define MAX_INAME_LEN     30	/* 选择列表项指示器的最大长度 */

pthread_mutex_t mutex_open_db;   /*同步创建数据库连接*/

void init_database()
{
	int i;
	EXEC SQL ENABLE THREADS;

	pthread_mutex_init(&mutex_open_db, NULL);

	for(i=0; i < MAX_ORA_CONNECTION_AMOUNT; i++)
	{
		EXEC SQL CONTEXT ALLOCATE :ora_connections[i];
		conn_status[i] = 0;
	}
}

int  closedb(int db_index)
{
	struct sqlca sqlca;

	if (db_index >= MAX_ORA_CONNECTION_AMOUNT || conn_status[db_index] != 1)
		return -1;

	EXEC SQL WHENEVER SQLERROR GOTO exception;
	EXEC SQL CONTEXT USE :ora_connections[db_index];
	EXEC SQL ROLLBACK WORK RELEASE;

	pthread_mutex_lock(&mutex_open_db);
	conn_status[db_index] = 0;
	pthread_mutex_unlock(&mutex_open_db);

	return 0;

exception:		/* 连接异常 */
	syslog(LOG_INFO, "close_db失败！dbindex[%d] dbtnsname[%s] errcode[%d]", db_index, ora_tnsname[db_index], sqlca.sqlcode);
	return -1;
}

int connect_db(const char* tnsname, const char* username, const char* password)
{
	struct sqlca sqlca;
	int db_index = -1;
	int i;
	char connect_string[256];
	sql_context ctx;

	if( tnsname != NULL && username != NULL && password != NULL)
	{
		sprintf(connect_string, "%s/%s@%s", username, password, tnsname);
		/*syslog(LOG_INFO, "%s/%s@%s", username, password, tnsname);*/
	}
	else
	{
		syslog(LOG_INFO, "connect_db函数参数错误！");
		return -1;
	}

	pthread_mutex_lock(&mutex_open_db);
	for(i = 0; i < MAX_ORA_CONNECTION_AMOUNT; i++)
	{
		if (conn_status[i] == 0)
		{
			db_index = i;
			break;
		}
	}

	if ( i == MAX_ORA_CONNECTION_AMOUNT)
	{
		pthread_mutex_unlock(&mutex_open_db);
		/*syslog(LOG_INFO, "连接上限！！！");*/
		return -1;
	}
	conn_status[db_index] = 1;
	pthread_mutex_unlock(&mutex_open_db);

	ctx = ora_connections[db_index];

	EXEC SQL WHENEVER SQLERROR GOTO exception1;
	/*EXEC SQL CONTEXT USE :ora_connections[db_index];*/
	EXEC SQL CONTEXT USE :ctx;
	EXEC SQL CONNECT :connect_string;
	EXEC SQL WHENEVER SQLERROR GOTO exception2;
	EXEC SQL ALTER SESSION SET NLS_DATE_FORMAT=YYYYMMDDHH24MISS;
	strcpy(ora_tnsname[db_index], tnsname);
	

	/*syslog(LOG_INFO, "connect success !!! dbtnsname[%s] conn_status[%d] dbindex[%d]", ora_tnsname[db_index], conn_status[db_index], db_index);*/
	return db_index;

exception1:		/* 连接异常 */
	pthread_mutex_lock(&mutex_open_db);
	conn_status[db_index] = 0;
	pthread_mutex_unlock(&mutex_open_db);
	syslog(LOG_INFO, "connect_db error , errcode[%d] dbtnsname[%s] conn_status[%d] dbindex[%d]", sqlca.sqlcode, tnsname, conn_status[db_index], db_index);
	return -1;

exception2:		/* 日期格式设定异常 */
	pthread_mutex_lock(&mutex_open_db);
	conn_status[db_index] = 0;
	pthread_mutex_unlock(&mutex_open_db);
	syslog(LOG_INFO, "connect_db set date format error , errcode[%d] dbtnsname[%s]", sqlca.sqlcode, tnsname);
	return -1;
}


int  exec_update(int db_index, const char* sql, int auto_commit)
{
	struct sqlca sqlca;
	if (db_index >= MAX_ORA_CONNECTION_AMOUNT || conn_status[db_index] != 1)
		return -1;

	EXEC SQL WHENEVER SQLERROR GOTO exception;
	EXEC SQL CONTEXT  USE :ora_connections[db_index];
	EXEC SQL EXECUTE IMMEDIATE :sql;

	if (auto_commit)
	{
		EXEC SQL COMMIT WORK;
	}

	return 0;

exception:
	syslog(LOG_INFO, "exec_update error, errcode[%d] dbtnsname[%s] conn_status[%d] db_index[%d]", sqlca.sqlcode, ora_tnsname[db_index], conn_status[db_index], db_index);
	syslog(LOG_INFO, "SQL[%s]", sql);

	return -1;
}

int  exec_query(int db_index, const char* sql, struct Proc_Result* pset)
{
	char   message[1024];
	int    i, null_ok, precision, scale;
	char*  p;
	struct sqlca sqlca;
	struct oraca oraca;
	SQLDA* select_dp;

	if (db_index >= MAX_ORA_CONNECTION_AMOUNT || conn_status[db_index] != 1)
		return -1;

	if ((select_dp = (SQLDA*)SQLSQLDAAlloc(&ora_connections[db_index], 
		MAX_ITEMS, MAX_VNAME_LEN, MAX_INAME_LEN)) == (SQLDA *) 0)
	{
		syslog(LOG_INFO, "在准备执行Oracle动态sql语句时，选择列表描述符分配失败！");
		return -1;
	}
	select_dp->N = MAX_ITEMS;

	/* Allocate the pointers to the indicator variables, and the actual data. */
	for (i = 0; i < MAX_ITEMS; i++)
	{
		select_dp->I[i] = (short *)malloc(sizeof(short));
		select_dp->V[i] = (char *)malloc(1);
	}

	EXEC SQL WHENEVER SQLERROR GOTO exception;
	EXEC SQL CONTEXT  USE :ora_connections[db_index];

	EXEC SQL PREPARE DYN_SQL FROM :sql;
	EXEC SQL DECLARE SQL_CUR CURSOR FOR DYN_SQL;
	EXEC SQL OPEN SQL_CUR;
	EXEC SQL DESCRIBE SELECT LIST FOR DYN_SQL INTO select_dp;

	pset->m_set_index = 0;
	pset->m_row_number = 0;
	pset->m_col_number = (int)select_dp->F;
	pset->m_row_total_length = 0;
	pset->m_pbuffer = NULL;
	pset->m_buffer_total = 0;

	/* Set the maximum number of array elements in the descriptor to the number found. */
	select_dp->N = select_dp->F;
	for (i = 0; i < select_dp->F; i++)
	{
		/* Turn off high-order bit of datatype. */
		sqlnult(&ora_connections[db_index], 
			(unsigned short *)&(select_dp->T[i]), (unsigned short *)&(select_dp->T[i]), &null_ok);

		switch (select_dp->T[i])
		{
		case  1 : /* CHAR datatype: no change in length needed, except possibly for TO_CHAR
				  conversions (not handled here). */
			select_dp->L[i] = select_dp->L[i] + 1;
			break;
		case  2 : /* NUMBER datatype: use sqlprc() to extract precision and scale. */
			sqlprct(&ora_connections[db_index],
				(unsigned long *)&(select_dp->L[i]), &precision, &scale);
			/* Allow for maximum size of NUMBER. */
			if (precision == 0) precision = 40;
			/* Also allow for decimal point and possible sign. */
			/* convert NUMBER datatype to FLOAT if scale > 0, INT otherwise. */
			if (scale > 0)
				select_dp->L[i] = sizeof(float);
			else
				select_dp->L[i] = sizeof(int);
			break;

		case  8 : /* LONG datatype */
			break;
		case 11 : /* ROWID datatype */
		case 104 : /* Universal ROWID datatype */
			select_dp->L[i] = 18;
			break;
		case 12 : /* DATE datatype */
			select_dp->L[i] = 9;
			break;
		case 23 : /* RAW datatype */
			break;
		case 24 : /* LONG RAW datatype */
			break;
		}
		/* Allocate space for the select-list data values. sqlald() reserves a pointer location for
		V[i] but does not allocate the full space for the pointer.  */

		if (select_dp->T[i] != 2)
			select_dp->V[i] = (char *) realloc(select_dp->V[i], select_dp->L[i] + 1);  
		else
			select_dp->V[i] = (char *) realloc(select_dp->V[i], select_dp->L[i]);  

		/* Coerce ALL datatypes except for LONG RAW and NUMBER and CHAR to character. */
		if (select_dp->T[i] != 24 && select_dp->T[i] != 2 && select_dp->T[i] != 96)
			select_dp->T[i] = 5;

		/* Coerce the datatypes of NUMBERs to float or int depending on the scale. */
		if (select_dp->T[i] == 2)
			if (scale > 0)		select_dp->T[i] = 4;  /* float */
			else				select_dp->T[i] = 3;  /* int */

		pset->m_col_type[i] = select_dp->T[i];
		pset->m_col_length[i] = select_dp->L[i];
		pset->m_row_total_length += pset->m_col_length[i];

		/*syslog(LOG_INFO, "type = %d, length = %d", pset->m_col_type[i], pset->m_col_length[i]);*/
	}

	/*syslog(LOG_INFO, "sql = %s", sql);*/

	/* FETCH each row selected and print the column values. */
	for (;;)
	{
		EXEC SQL FETCH SQL_CUR USING DESCRIPTOR select_dp;
		if (sqlca.sqlcode < 0 || sqlca.sqlcode == 1403)
			break;
/*
syslog(LOG_INFO, "fetch items = %d sqlcode = %d", pset->m_row_number + 1, sqlca.sqlcode);
*/
		if (pset->m_row_number >= pset->m_buffer_total)
		{
			pset->m_buffer_total += 1024;
			pset->m_pbuffer = (char *) realloc(pset->m_pbuffer,
				pset->m_buffer_total * pset->m_row_total_length);
			if (pset->m_pbuffer == NULL) 
			{
				syslog(LOG_ALERT, "分配内存出错！");
				return -1;
			}
		}
		p = pset->m_pbuffer + pset->m_row_number * pset->m_row_total_length;
		memset(p, 0, pset->m_row_total_length);

		/* Since each variable returned has been coerced to a
		character string, int, or float very little processing 
		is required here.  This routine just prints out the 
		values on the terminal. */
		for (i = 0; i < select_dp->F; i++)
		{
			if (*select_dp->I[i] < 0)
				p += pset->m_col_length[i];
			else
			{
				if (select_dp->T[i] == 3)     /* int datatype */
				{
					*(int*)p = *(int *)select_dp->V[i];
/*
					syslog(LOG_INFO, "int value = %d, int*p = %d", *(int *)select_dp->V[i], *(int*)p);
*/
					p += sizeof(int);
				}
				else if (select_dp->T[i] == 4)/* float datatype */
				{
					*(float*)p = *(float *)select_dp->V[i];
					p += sizeof(float);
				}
				else                          /* character string */
				{
					memset(message, 0, 1024);
					strncpy(message, select_dp->V[i], pset->m_col_length[i]);
					/*syslog(LOG_INFO, "string value = %s", message);*/
					select_dp->V[i][pset->m_col_length[i]] = '\0';
					memcpy(p, select_dp->V[i], pset->m_col_length[i]);
					p += pset->m_col_length[i];
				}
			}
		}
		pset->m_row_number++;
	}

    for (i = 0; i < MAX_ITEMS; i++)
    {    
        if (select_dp->V[i] != (char *) 0)
            free(select_dp->V[i]);
        free(select_dp->I[i]); /* MAX_ITEMS were allocated. */
    }
    SQLSQLDAFree(&ora_connections[db_index], select_dp);
    EXEC SQL CLOSE SQL_CUR;

	return 0;

exception:
	syslog(LOG_INFO, "exec_query error");
	syslog(LOG_INFO, "SQL[%s]", sql);
	syslog(LOG_ERR, "Last SQL: %.*s", oraca.orastxt.orastxtl, oraca.orastxt.orastxtc);
	syslog(LOG_ERR, "%.*s", sqlca.sqlerrm.sqlerrml, sqlca.sqlerrm.sqlerrmc);
	return -1;
}

int  roll_back(int db_index)
{
	struct sqlca sqlca;
	if (db_index >= MAX_ORA_CONNECTION_AMOUNT || conn_status[db_index] != 1)
		return -1;

	EXEC SQL WHENEVER SQLERROR GOTO exception;
	EXEC SQL CONTEXT USE :ora_connections[db_index];
	EXEC SQL ROLLBACK WORK;

	return 0;

exception:		/* 连接异常 */
	syslog(LOG_INFO, "rollback error, errcode[%d] dbtnsname[%s] conn_status[%d] db_index[%d]", sqlca.sqlcode, ora_tnsname[db_index], conn_status[db_index], db_index);
	return -1;
}

int  commit(int db_index)
{
	struct sqlca sqlca;
	if (db_index >= MAX_ORA_CONNECTION_AMOUNT || conn_status[db_index] != 1)
		return -1;

	EXEC SQL WHENEVER SQLERROR GOTO exception;
	EXEC SQL CONTEXT USE :ora_connections[db_index];
	EXEC SQL COMMIT WORK;

	return 0;

exception:		/* 连接异常 */
	syslog(LOG_INFO, "commit error, errcode[%d] dbtnsname[%s] conn_status[%d] db_index[%d]", sqlca.sqlcode, ora_tnsname[db_index], conn_status[db_index], db_index);
	return -1;
}

