#ifndef __PROC_UTILS_H__
#define __PROC_UTILS_H__

#ifdef __cplusplus
extern "C"{
#endif

struct Proc_Result
{
	int   m_set_index;
	int   m_row_number;
	int   m_col_number;
	short m_col_type[40];
	int   m_col_length[40];
	int   m_row_total_length;
	char* m_pbuffer;
	int   m_buffer_total;
};

void init_database();
int  connect_db(const char* tnsname, const char* username, const char* password);
int  exec_update(int db_index, const char* sql, int auto_commit);
int  exec_query(int db_index, const char* sql, struct Proc_Result* pset);
int  roll_back(int db_index);
int  commit(int db_index);
int  closedb(int db_index);

#ifdef __cplusplus
}
#endif

#endif /* __PROC_UTILS_H__ */
