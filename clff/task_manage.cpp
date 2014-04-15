#include "header.h"
#include "task_receive.h"
#include "policy_send.h"
#include "Database.h"

// 分中心ID
int  g_fzx_id;
char LocalIP[64];

// 数据库配置信息
DbInfo gjzb;
DbInfo gjyd;//add by ma 20100210
DbInfo fzx1;
DbInfo fzx2;

// 任务检测时间间隔
int  g_check_interval;

// 链路探测时间
int g_link_test_time;

// 策略分发目标信息
vector<TargetInfo*> g_dzjs_targets;
vector<TargetInfo*> g_nrjs_targets;
vector<TargetInfo*> g_dzbh_targets;
vector<TargetInfo*> g_dzfx_targets;
vector<TargetInfo*> g_dzfd_targets;
vector<TargetInfo*> g_nrfd_targets;
vector<TargetInfo*> g_nrjc_targets;
vector<TargetInfo*> g_hhdzjc_targets;
vector<TargetInfo*> g_hhnrjc_targets;
vector<TargetInfo*> g_glzf_targets;

// 待发送策略的版本信息
char g_version[100];
int  g_policy_type;

// 待发送的策略信息
list<DzfxPolicy*> g_dzfx_buff;
list<DzfdPolicy*> g_dzfd_buff;
list<DzjkPolicy*> g_dzjk_buff;
list<NrjkPolicy*> g_nrjk_buff;
list<GlzfPolicy*> g_glzf_buff;

// 策略分发线程相关
int  g_task_target_count;
int  g_task_finish_count;
pthread_mutex_t mutex_dispatch;
pthread_cond_t  cond_dispatch;

// 策略分发线程执行结果
int* policy_send_result = NULL;
int gjzx_switch_yz = 0; // add by ma 20100210

/*
原型	bool task_manage_init(ArgFile* pFile)
功能	从配置文件读取配置信息，对模块进行初始化
参数	pFile 配置信息读取类指针 
返回值  true 成功， false 失败
*/
bool task_manage_init(ArgFile* pFile)
{
	struct ARG*			pArg;
	struct ARG_SECTION* section;

	memset(&gjzb, 0, sizeof(DbInfo));
	memset(&gjyd, 0, sizeof(DbInfo));//add by ma 20100210
	//memset(&gjyd, 0, sizeof(DbInfo));
	memset(&fzx1, 0, sizeof(DbInfo));
	memset(&fzx2, 0, sizeof(DbInfo));

	const char *pIP;
	char ptr[200];
	char str[32];
	struct hostent *hptr;
	gethostname(ptr, 200);
	hptr = gethostbyname(ptr);
	memset(LocalIP, 0, 64);
	pIP = inet_ntop(hptr->h_addrtype, *hptr->h_addr_list, str, sizeof(str));
	memcpy(LocalIP, pIP, 64);

	if(pArg = pFile->GetArg("SERVER", "FZX_ID"))
		g_fzx_id = atoi(pArg->argvalue);
	else
		return false;
	// 异地数据库是否启用标志 add by ma 20100210
	if(pArg = pFile->GetArg("YZ_SWITCH", "GJZX_SWITCH_YZ"))
		gjzx_switch_yz = atoi(pArg->argvalue);
	else
		return false;

	if(pArg = pFile->GetArg("TASK_MANAGER", "LINK_TEST_TIME"))
		g_link_test_time = atoi(pArg->argvalue);
	else
		return false;
	// 从打开的配置文件中初始化异地数据库连接信息 add by ma 20100210
	if(pArg = pFile->GetArg("POLICY_GJDB_YD", "TNSNAME"))
		strcpy(gjyd.tnsname, pArg->argvalue);
	else
		return false;
	if(pArg = pFile->GetArg("POLICY_GJDB_YD", "USER"))
		strcpy(gjyd.user, pArg->argvalue);
	else
		return false;
	if(pArg = pFile->GetArg("POLICY_GJDB_YD", "PASSWORD"))
		strcpy(gjyd.pass, pArg->argvalue);
	else
		return false;

	// 从打开的配置文件中初始化国家灾备数据库连接信息
	if(pArg = pFile->GetArg("POLICY_GJDB_ZB", "TNSNAME"))
		strcpy(gjzb.tnsname, pArg->argvalue);
	else
		return false;
	if(pArg = pFile->GetArg("POLICY_GJDB_ZB", "USER"))
		strcpy(gjzb.user, pArg->argvalue);
	else
		return false;
	if(pArg = pFile->GetArg("POLICY_GJDB_ZB", "PASSWORD"))
		strcpy(gjzb.pass, pArg->argvalue);
	else
		return false;

	// 从打开的配置文件中初始化分中心1号数据库连接信息
	if(pArg = pFile->GetArg("POLICY_FZXDB_MAIN", "TNSNAME"))
		strcpy(fzx1.tnsname, pArg->argvalue);
	else
		return false;
	if(pArg = pFile->GetArg("POLICY_FZXDB_MAIN", "USER"))
		strcpy(fzx1.user, pArg->argvalue);
	else
		return false;
	if(pArg = pFile->GetArg("POLICY_FZXDB_MAIN", "PASSWORD"))
		strcpy(fzx1.pass, pArg->argvalue);
	else
		return false;
	// 从打开的配置文件中初始化分中心2号数据库连接信息
	if(pArg = pFile->GetArg("POLICY_FZXDB_BACK", "TNSNAME"))
		strcpy(fzx2.tnsname, pArg->argvalue);
	else
		return false;
	if(pArg = pFile->GetArg("POLICY_FZXDB_BACK", "USER"))
		strcpy(fzx2.user, pArg->argvalue);
	else
		return false;
	if(pArg = pFile->GetArg("POLICY_FZXDB_BACK", "PASSWORD"))
		strcpy(fzx2.pass, pArg->argvalue);
	else
		return false;

	if(pArg = pFile->GetArg("TASK_MANAGER", "CHECK_INTERVAL"))
		g_check_interval = atoi(pArg->argvalue);
	else
		return false;

	section = pFile->GetSection("TASK_MANAGE/CL_DZJS");	
	if(section != NULL)
	{
		pArg = pFile->GetArg(section);
		while(pArg)
		{
			int				counter= 0;

			while(pArg->argvalue[counter] && pArg->argvalue[counter] != ':'){counter ++;}
			if(counter > 0 && pArg->argvalue[counter] )
			{
				TargetInfo *pTargetInfo = new TargetInfo();
				memset(pTargetInfo, 0, sizeof(TargetInfo));
				memcpy(pTargetInfo->ip, pArg->argvalue, counter);
				pTargetInfo->port = (unsigned short) atoi(&pArg->argvalue[counter+1]);
				g_dzjs_targets.push_back(pTargetInfo);
			}
			else
				return false;
			pArg = pArg->next;
		}
	}
	else
	{
		trapmsg_error_fmt(CENO_FILE_FORMATE_ERROR,"文件格式错误 文件名:%s\n", pFile->GetFileName());
		return false;
	}
	section = pFile->GetSection("TASK_MANAGE/CL_NRJS");
	if(section != NULL)
	{
		pArg = pFile->GetArg(section);
		while(pArg)
		{
			int				counter= 0;
			while(pArg->argvalue[counter] && pArg->argvalue[counter] != ':'){counter ++;}
			if(counter > 0 && pArg->argvalue[counter] )
			{
				TargetInfo *pTargetInfo = new TargetInfo();
				memset(pTargetInfo, 0, sizeof(TargetInfo));
				memcpy(pTargetInfo->ip, pArg->argvalue, counter);
				pTargetInfo->port = (unsigned short) atoi(&pArg->argvalue[counter+1]);
				g_nrjs_targets.push_back(pTargetInfo);
			}
			else
				return false;
			pArg = pArg->next;
		}
	}
	else
	{
		trapmsg_error_fmt(CENO_FILE_FORMATE_ERROR,"文件格式错误 文件名:%s\n", pFile->GetFileName());
		return false;
	}

	section = pFile->GetSection("TASK_MANAGE/CL_GLZF");
	if(section != NULL)
	{
		pArg = pFile->GetArg(section);
		while(pArg)
		{
			int				counter= 0;
			while(pArg->argvalue[counter] && pArg->argvalue[counter] != ':'){counter ++;}
			if(counter > 0 && pArg->argvalue[counter] )
			{
				TargetInfo *pTargetInfo = new TargetInfo();
				memset(pTargetInfo, 0, sizeof(TargetInfo));
				memcpy(pTargetInfo->ip, pArg->argvalue, counter);
				pTargetInfo->port = (unsigned short) atoi(&pArg->argvalue[counter+1]);
				g_glzf_targets.push_back(pTargetInfo);
			}
			else
				return false;
			pArg = pArg->next;
		}
	}	
	else
	{
		trapmsg_error_fmt(CENO_FILE_FORMATE_ERROR,"文件格式错误 文件名:%s\n", pFile->GetFileName());
		return false;
	}


	section = pFile->GetSection("TASK_MANAGE/CL_DZBH");
	if(section != NULL)
	{
		pArg = pFile->GetArg(section);
		while(pArg)
		{
			int				counter= 0;
			while(pArg->argvalue[counter] && pArg->argvalue[counter] != ':'){counter ++;}
			if(counter > 0 && pArg->argvalue[counter] )
			{
				TargetInfo *pTargetInfo = new TargetInfo();
				memset(pTargetInfo, 0, sizeof(TargetInfo));
				memcpy(pTargetInfo->ip, pArg->argvalue, counter);
				pTargetInfo->port = (unsigned short) atoi(&pArg->argvalue[counter+1]);
				g_dzbh_targets.push_back(pTargetInfo);
			}
			else
				return false;
			pArg = pArg->next;
		}
	}	
	else
	{
		trapmsg_error_fmt(CENO_FILE_FORMATE_ERROR,"文件格式错误 文件名:%s\n", pFile->GetFileName());
		return false;
	}

	section = pFile->GetSection("TASK_MANAGE/CL_DZFX");
	if(section != NULL)
	{
		pArg = pFile->GetArg(section);
		while(pArg)
		{
			int				counter= 0;
			while(pArg->argvalue[counter] && pArg->argvalue[counter] != ':'){counter ++;}
			if(counter > 0 && pArg->argvalue[counter] )
			{
				TargetInfo *pTargetInfo = new TargetInfo();
				memset(pTargetInfo, 0, sizeof(TargetInfo));
				memcpy(pTargetInfo->ip, pArg->argvalue, counter);
				pTargetInfo->port = (unsigned short) atoi(&pArg->argvalue[counter+1]);
				g_dzfx_targets.push_back(pTargetInfo);
			}
			else
				return false;
			pArg = pArg->next;
		}
	}	
	else
	{
		trapmsg_error_fmt(CENO_FILE_FORMATE_ERROR,"文件格式错误 文件名:%s\n", pFile->GetFileName());
		return false;
	}
	section = pFile->GetSection("TASK_MANAGE/CL_DZFD");
	if(section != NULL)
	{
		pArg = pFile->GetArg(section);
		while(pArg)
		{
			int				counter= 0;
			while(pArg->argvalue[counter] && pArg->argvalue[counter] != ':'){counter ++;}
			if(counter > 0 && pArg->argvalue[counter] )
			{
				TargetInfo *pTargetInfo = new TargetInfo();
				memset(pTargetInfo, 0, sizeof(TargetInfo));
				memcpy(pTargetInfo->ip, pArg->argvalue, counter);
				pTargetInfo->port = (unsigned short) atoi(&pArg->argvalue[counter+1]);
				g_dzfd_targets.push_back(pTargetInfo);
			}
			else
				return false;
			pArg = pArg->next;
		}
	}
	else
	{
		trapmsg_error_fmt(CENO_FILE_FORMATE_ERROR,"文件格式错误 文件名:%s\n", pFile->GetFileName());
		return false;
	}
	section = pFile->GetSection("TASK_MANAGE/CL_NRFD");
	if(section != NULL)
	{
		pArg = pFile->GetArg(section);
		while(pArg)
		{
			int				counter= 0;
			while(pArg->argvalue[counter] && pArg->argvalue[counter] != ':'){counter ++;}
			if(counter > 0 && pArg->argvalue[counter] )
			{
				TargetInfo *pTargetInfo = new TargetInfo();
				memset(pTargetInfo, 0, sizeof(TargetInfo));
				memcpy(pTargetInfo->ip, pArg->argvalue, counter);
				pTargetInfo->port = (unsigned short) atoi(&pArg->argvalue[counter+1]);
				g_nrfd_targets.push_back(pTargetInfo);
			}
			else
				return false;
			pArg = pArg->next;
		}
	}
	else
	{
		trapmsg_error_fmt(CENO_FILE_FORMATE_ERROR,"文件格式错误 文件名:%s\n", pFile->GetFileName());
		return false;
	}
	section = pFile->GetSection("TASK_MANAGE/CL_NRJC");
	if(section != NULL)
	{
		pArg = pFile->GetArg(section);
		while(pArg)
		{
			int				counter= 0;

			while(pArg->argvalue[counter] && pArg->argvalue[counter] != ':'){counter ++;}
			if(counter > 0 && pArg->argvalue[counter] )
			{
				TargetInfo *pTargetInfo = new TargetInfo();
				memset(pTargetInfo, 0, sizeof(TargetInfo));			
				memcpy(pTargetInfo->ip, pArg->argvalue, counter);
				pTargetInfo->port = (unsigned short) atoi(&pArg->argvalue[counter+1]);
				g_nrjc_targets.push_back(pTargetInfo);
			}
			else
				return false;
			pArg = pArg->next;
		}
	}
	else
	{
		trapmsg_error_fmt(CENO_FILE_FORMATE_ERROR,"文件格式错误 文件名:%s\n", pFile->GetFileName());
		return false;
	}

	section = pFile->GetSection("TASK_MANAGE/CL_HHDZJC");
	if(section != NULL)
	{
		pArg = pFile->GetArg(section);
		while(pArg)
		{
			int				counter= 0;
			while(pArg->argvalue[counter] && pArg->argvalue[counter] != ':'){counter ++;}
			if(counter > 0 && pArg->argvalue[counter] )
			{
				TargetInfo *pTargetInfo = new TargetInfo();
				memset(pTargetInfo, 0, sizeof(TargetInfo));
				memcpy(pTargetInfo->ip, pArg->argvalue, counter);
				pTargetInfo->port = (unsigned short) atoi(&pArg->argvalue[counter+1]);
				g_hhdzjc_targets.push_back(pTargetInfo);
			}
			else
				return false;
			pArg = pArg->next;
		}
	}
	else
	{
		trapmsg_error_fmt(CENO_FILE_FORMATE_ERROR,"文件格式错误 文件名:%s\n", pFile->GetFileName());
		return false;
	}

	section = pFile->GetSection("TASK_MANAGE/CL_HHNRJC");
	if(section != NULL)
	{
		pArg = pFile->GetArg(section);
		while(pArg)
		{
			int				counter= 0;

			while(pArg->argvalue[counter] && pArg->argvalue[counter] != ':'){counter ++;}

			if(counter > 0 && pArg->argvalue[counter] )
			{
				TargetInfo *pTargetInfo = new TargetInfo();
				memset(pTargetInfo, 0, sizeof(TargetInfo));
				memcpy(pTargetInfo->ip, pArg->argvalue, counter);
				pTargetInfo->port = (unsigned short) atoi(&pArg->argvalue[counter+1]);
				g_hhnrjc_targets.push_back(pTargetInfo);
			}
			else
				return false;
			pArg = pArg->next;
		}
	}
	else
	{
		trapmsg_error_fmt(CENO_FILE_FORMATE_ERROR,"文件格式错误 文件名:%s\n", pFile->GetFileName());
		return false;
	}

	return true;
}


// 此函数用于查询db1、db2表示的各库，已获得当前指定中心配置主库连接
// Flag = 1 表示db1、db2为zx_id所表示的中心的配置库1和配置库2
// Flag = 0 表示db1、db2不为zx_id所表示的中心的配置库1和配置库2
Database* get_pzdb(DbInfo* db1, DbInfo* db2, int zx_id, int flag)
{
	DbInfo	fzx_db_main1;
	DbInfo	fzx_db_main2;
	char	sql[128];
	bool	db1_ok ,db2_ok;
	db1_ok = db2_ok = true;

	Database* szx_db1 = new Database(ora_server);
	Database* szx_db2 = new Database(ora_server);

	if(szx_db1->Open(db1->tnsname , db1->user, db1->pass) == false)
	{
		trapmsg_error_fmt(CENO_DB_CONN_ERROR_STR2,"1号配置库连接失败！");
		db1_ok = false;
		szx_db1->Close();
		delete szx_db1;
	}
	if(szx_db2->Open(db2->tnsname , db2->user, db2->pass) == false)
	{
		trapmsg_error_fmt(CENO_DB_CONN_ERROR_STR2,"2号配置库连接失败！");
		db2_ok = false;
		szx_db2->Close();
		delete szx_db2;
	}
	if(db1_ok == false && db2_ok == false)
	{
		return NULL;
	}
	else if (db1_ok != false && db2_ok != false)
	{
		sprintf(sql, "select fwm, yhm, mm from dmb_dbinfo where zxid = %d and zb = 1", zx_id);
		Recordset* pSet1 = new Recordset(szx_db1);
		//pSet1 = szx_db1->ExecQuery(sql);
		if(!pSet1->Open(sql))
		{
			trapmsg_error_fmt(CENO_DB_CONN_ERROR_STR2,"从配置1号库中获取本地分中心配置主库信息失败！");
			pSet1->Close();
			delete pSet1;
			szx_db1->Close();
			delete szx_db1;
			return NULL;
		}
		if(pSet1->IsEOF() == false)
		{
			memset(fzx_db_main1.tnsname, 0, sizeof(fzx_db_main1.tnsname));
			memset(fzx_db_main1.user, 0, sizeof(fzx_db_main1.user));
			memset(fzx_db_main1.pass, 0, sizeof(fzx_db_main1.pass));
			strcpy(fzx_db_main1.tnsname, pSet1->GetStringValue(1).c_str());
			strcpy(fzx_db_main1.user, pSet1->GetStringValue(2).c_str());
			strcpy(fzx_db_main1.pass, pSet1->GetStringValue(3).c_str());				

		}
		else
		{
			trapmsg_error_fmt(CENO_DB_CONN_ERROR_STR2,"从配置1号库中获取本地分中心配置主库信息失败！");
			pSet1->Close();
			delete pSet1;
			szx_db1->Close();
			delete szx_db1;
			return NULL;
		}
		pSet1->Close();
		delete pSet1;

		sprintf(sql, "select fwm, yhm, mm from dmb_dbinfo where zxid = %d and zb = 1", zx_id);
		Recordset* pSet2 = new Recordset(szx_db2);
		//pSet2 = szx_db2->ExecQuery(sql);
		if(!pSet2->Open(sql))
		{
			trapmsg_error_fmt(CENO_DB_CONN_ERROR_STR2,"从配置2号库中获取本地分中心配置主库信息失败！");
			pSet2->Close();
			delete pSet2;
			szx_db2->Close();
			delete szx_db2;
			return NULL;
		}
		if(pSet2->IsEOF() == false)
		{
			memset(fzx_db_main2.tnsname, 0, sizeof(fzx_db_main2.tnsname));
			memset(fzx_db_main2.user, 0, sizeof(fzx_db_main2.user));
			memset(fzx_db_main2.pass, 0, sizeof(fzx_db_main2.pass));
			strcpy(fzx_db_main2.tnsname, pSet2->GetStringValue(1).c_str());
			strcpy(fzx_db_main2.user, pSet2->GetStringValue(2).c_str());
			strcpy(fzx_db_main2.pass, pSet2->GetStringValue(3).c_str());				

		}
		else
		{
			trapmsg_error_fmt(CENO_DB_CONN_ERROR_STR2,"从配置2号库中获取本地分中心配置主库信息失败！");
			pSet2->Close();
			delete pSet2;
			szx_db2->Close();
			delete szx_db2;
			return NULL;
		}
		pSet2->Close();
		delete pSet2;

		if((strcasecmp(fzx_db_main1.tnsname, fzx_db_main2.tnsname) == 0) &&
			(strcasecmp(fzx_db_main1.user, fzx_db_main2.user) == 0) &&
			(strcasecmp(fzx_db_main1.pass, fzx_db_main2.pass) == 0))
		{
			if(strcasecmp(fzx_db_main1.tnsname, db1->tnsname) == 0 &&
				strcasecmp(fzx_db_main1.user, db1->user) == 0 &&
				strcasecmp(fzx_db_main1.pass, db1->pass) == 0)
			{
				szx_db2->Close();
				delete szx_db2;
				return szx_db1;
			}
			else
			{
				szx_db1->Close();
				delete szx_db1;
				return szx_db2;
			}
		}
		else
		{
			trapmsg_error_fmt(CENO_DB_CONN_ERROR_STR2,"本省配置1号配置库与2号配置库dmb_dbinfo中的本地分中心配置主库信息不一致！");
			szx_db1->Close();
			delete szx_db1;
			szx_db2->Close();
			delete szx_db2;
			return NULL;
		}
	}
	else if(db1_ok)
	{
		delete szx_db2;
		sprintf(sql, "select fwm, yhm, mm from dmb_dbinfo where zxid = %d and zb = 1", zx_id);
		Recordset* pSet1 = new Recordset(szx_db1);
		//pSet1 = szx_db1->ExecQuery(sql);
		if(!pSet1->Open(sql))
		{
			trapmsg_error_fmt(CENO_DB_CONN_ERROR_STR2,"从配置1号库中获取本地分中心配置主库信息失败！");
			pSet1->Close();
			delete pSet1;
			szx_db1->Close();
			delete szx_db1;
			return NULL;
		}
		if(pSet1->IsEOF() == false)
		{
			memset(fzx_db_main1.tnsname, 0, sizeof(fzx_db_main1.tnsname));
			memset(fzx_db_main1.user, 0, sizeof(fzx_db_main1.user));
			memset(fzx_db_main1.pass, 0, sizeof(fzx_db_main1.pass));
			strcpy(fzx_db_main1.tnsname, pSet1->GetStringValue(1).c_str());
			strcpy(fzx_db_main1.user, pSet1->GetStringValue(2).c_str());
			strcpy(fzx_db_main1.pass, pSet1->GetStringValue(3).c_str());				

			if(strcasecmp(fzx_db_main1.tnsname, db1->tnsname) == 0 &&
				strcasecmp(fzx_db_main1.user, db1->user) == 0 &&
				strcasecmp(fzx_db_main1.pass, db1->pass) == 0)
			{
				return szx_db1;
			}
			else
			{
				trapmsg_error_fmt(CENO_DB_CONN_ERROR_STR2,"本省配置1号配置库与本地系统配置文件中的本地分中心配置主库信息不一致！");
				szx_db1->Close();
				delete szx_db1;
				return NULL;
			}
		}
		else
		{
			trapmsg_error_fmt(CENO_DB_CONN_ERROR_STR2,"从配置1号库中获取本地分中心配置主库信息失败！");
			pSet1->Close();
			delete pSet1;
			szx_db1->Close();
			delete szx_db1;
			return NULL;
		}
	}
	else if(db2_ok)
	{
		sprintf(sql, "select fwm, yhm, mm from dmb_dbinfo where zxid = %d and zb = 1", zx_id);
		Recordset* pSet2 = new Recordset(szx_db2);
		//pSet1 = szx_db1->ExecQuery(sql);
		if(!pSet2->Open(sql))
		{
			trapmsg_error_fmt(CENO_DB_CONN_ERROR_STR2,"从配置1号库中获取本地分中心配置主库信息失败！");
			pSet2->Close();
			delete pSet2;
			szx_db2->Close();
			delete szx_db2;
			return NULL;
		}
		if(pSet2->IsEOF() == false)
		{
			memset(fzx_db_main2.tnsname, 0, sizeof(fzx_db_main2.tnsname));
			memset(fzx_db_main2.user, 0, sizeof(fzx_db_main2.user));
			memset(fzx_db_main2.pass, 0, sizeof(fzx_db_main2.pass));
			strcpy(fzx_db_main2.tnsname, pSet2->GetStringValue(1).c_str());
			strcpy(fzx_db_main2.user, pSet2->GetStringValue(2).c_str());
			strcpy(fzx_db_main2.pass, pSet2->GetStringValue(3).c_str());				

			if(strcasecmp(fzx_db_main2.tnsname, db2->tnsname) == 0 &&
				strcasecmp(fzx_db_main2.user, db2->user) == 0 &&
				strcasecmp(fzx_db_main2.pass, db2->pass) == 0)
			{
				return szx_db2;
			}
			else
			{
				trapmsg_error_fmt(CENO_DB_CONN_ERROR_STR2,"配置1号库中本地分中心配置主库信息与本地配置文件中配置主库信息不一致！");
				szx_db2->Close();
				delete szx_db2;
				return NULL;
			}
		}
		else
		{
			trapmsg_error_fmt(CENO_DB_CONN_ERROR_STR2,"从配置1号库中获取本地分中心配置主库信息失败！");
			pSet2->Close();
			delete pSet2;
			szx_db2->Close();
			delete szx_db2;
			return NULL;
		}
	}
	else
	{
		trapmsg_error_fmt(CENO_DB_CONN_ERROR_STR2,"连接分中心配置主库失败！");
		return NULL;
	}
}



// 此函数用于确定当前分中心配置主库信息
// 此函数用于查询gj_db表示的各库，已获得当前指定中心配置主库连接
// Flag = 1 表示gj_db为fzx_id所表示的中心的配置库1和配置库2
// Flag = 0 表示gj_db不为fzx_id所表示的中心的配置库1和配置库2
Database* get_pzdb(DbInfo* gj_db, int fzx_id, int switch_yz, int flag)
{
	char sql[512];
	DbInfo fzx_db_main;

	if(switch_yz == 0)
	{
		Database* gjzx_db = new Database(yd_server);	// 国家中心异地接口
		// 连接国家中心配置主库
		if(gjzx_db->Open(gj_db->tnsname , gj_db->user, gj_db->pass) == false)
		{
			trapmsg_error_fmt(CENO_DB_CONN_ERROR_STR2,"连接国家中心大型库失败，开始使用本地配置文件中的配置信息连接本地分中心配置主库！");
			//gjzx_db->Close();
			delete gjzx_db;
			return NULL;
		}
		else
		{
			if(flag == 1)
				return gjzx_db;

			// 从国家库中确定本地分中心的配置主库信息
			sprintf(sql, "select fwm,yhm,mm from dmb_dbinfo where zxid = %d and zb = 1", fzx_id);
			Recordset* pSet = new Recordset(gjzx_db);
			//pSet = gjzx_db->ExecQuery(sql);
			if(!pSet->Open(sql))
			{
				trapmsg_error_fmt(CENO_DB_CONN_ERROR_STR2,"从国家大型库中获取本地分中心配置主库信息失败！");
				pSet->Close();
				delete pSet;
				gjzx_db->Close();
				delete gjzx_db;
				return NULL;
			}
			if(pSet->IsEOF() == false)
			{
				memset(fzx_db_main.tnsname, 0, sizeof(fzx_db_main.tnsname));
				memset(fzx_db_main.user, 0, sizeof(fzx_db_main.user));
				memset(fzx_db_main.pass, 0, sizeof(fzx_db_main.pass));
				strcpy(fzx_db_main.tnsname, pSet->GetStringValue(1).c_str());
				strcpy(fzx_db_main.user, pSet->GetStringValue(2).c_str());
				strcpy(fzx_db_main.pass, pSet->GetStringValue(3).c_str());
			}
			else
			{
				trapmsg_error_fmt(CENO_DB_CONN_ERROR_STR2,"从国家大型库中获取本地分中心配置主库信息失败！");
				pSet->Close();
				delete pSet;
				gjzx_db->Close();
				delete gjzx_db;
				return NULL;
			}
			pSet->Close();
			delete pSet;
			gjzx_db->Close();
			delete gjzx_db;
			
			Database* szx_db = new Database(ora_server);
			if(szx_db->Open(fzx_db_main.tnsname,fzx_db_main.user,fzx_db_main.pass) == true)
			{
				trapmsg_normal_fmt("用从国家大型库中获取的本地分中心主库配置库信息，连接本地分中心主库成功！");
				return szx_db;
			}
			else 
			{
				trapmsg_normal_fmt("用从国家大型库中获取的本地分中心主库配置库信息，连接分中心主库失败！请检查异地国家主库中配置信息是否正确，分中心主库是否正常！");
				szx_db->Close();
				delete szx_db;
				return NULL;
			}
		}
	}
	else
	{
		Database* gjzx_db = new Database(ora_server);    //proc接口
		// 连接国家中心配置主库
		if(gjzx_db->Open(gj_db->tnsname , gj_db->user, gj_db->pass) == false)
		{
			trapmsg_error_fmt(CENO_DB_CONN_ERROR_STR2,"连接异地国家中心配置库失败，开始使用本地配置文件中的配置信息连接本地分中心配置主库！");
			//gjzx_db->Close();
			delete gjzx_db;
			return NULL;
		}
		else
		{
			if(flag == 1)
				return gjzx_db;

			// 从国家库中确定本地分中心的配置主库信息
			sprintf(sql, "select fwm,yhm,mm from dmb_dbinfo where zxid = %d and zb = 1", fzx_id);
			Recordset* pSet = new Recordset(gjzx_db);
			//pSet = gjzx_db->ExecQuery(sql);
			if(!pSet->Open(sql))
			{
				trapmsg_error_fmt(CENO_DB_CONN_ERROR_STR2,"从国家大型库中获取本地分中心配置主库信息失败！");
				pSet->Close();
				delete pSet;
				gjzx_db->Close();
				delete gjzx_db;
				return NULL;
			}
			if(pSet->IsEOF() == false)
			{
				memset(fzx_db_main.tnsname, 0, sizeof(fzx_db_main.tnsname));
				memset(fzx_db_main.user, 0, sizeof(fzx_db_main.user));
				memset(fzx_db_main.pass, 0, sizeof(fzx_db_main.pass));
				strcpy(fzx_db_main.tnsname, pSet->GetStringValue(1).c_str());
				strcpy(fzx_db_main.user, pSet->GetStringValue(2).c_str());
				strcpy(fzx_db_main.pass, pSet->GetStringValue(3).c_str());
			}
			else
			{
				trapmsg_error_fmt(CENO_DB_CONN_ERROR_STR2,"从国家大型库中获取本地分中心配置主库信息失败！");
				pSet->Close();
				delete pSet;
				gjzx_db->Close();
				delete gjzx_db;
				return NULL;
			}
			pSet->Close();
			delete pSet;
			gjzx_db->Close();
			delete gjzx_db;

			Database* szx_db = new Database(ora_server);
			if(szx_db->Open(fzx_db_main.tnsname,fzx_db_main.user,fzx_db_main.pass) == true)
			{
				trapmsg_normal_fmt("使用从国家大型库中获取的本地分中心主库信息，连接本地分中心主库成功！");
				return szx_db;
			}
			else 
			{
				trapmsg_normal_fmt("使用从国家大型库中获取的本地分中心主库信息，连接本地分中心主库失败！请检查国家大型库中配置信息是否正确或分中心主库是否正常！");
				szx_db->Close();
				delete szx_db;
				return NULL;
			}
		}
	}
}

/*
原型	bool read_version(Connection* pConn, int type)
功能	从数据库读取策略版本信息
参数	pConn 与数据库的连接标志指针， type 策略类型 
返回值  true 成功， false 失败
*/
bool read_version(Database* szx_db, int type)
{
	memset(g_version, 0,sizeof(g_version));

	return true;
}

/*
原型	bool read_nrjk_policy(Connection* pConn, char* sql)
功能	从数据库读取地址放行策略
参数	pConn 与数据库的连接标志指针， sql 读取策略的sql语句指针 
返回值  true 成功， false 失败
*/
bool read_glzf_policy(Database* szx_db, char* sql)
{
	if (g_glzf_buff.size() > 0)
	{
		list<GlzfPolicy*>::iterator iter;
		for(iter = g_glzf_buff.begin(); iter != g_glzf_buff.end(); iter++)
			delete *iter;
		g_glzf_buff.clear();
	}

	Recordset* pSet = new Recordset(szx_db);
	if(!pSet->Open(sql))
		return false;

	while(pSet->IsEOF() == false)
	{
		GlzfPolicy* p = new GlzfPolicy();
		p->id = pSet->GetIntValue(1);
		string tmp;
		tmp = pSet->GetStringValue(2);
		strcpy(p->zfz, tmp.c_str());
		tmp = pSet->GetStringValue(3);
		strcpy(p->bz, tmp.c_str());

		g_glzf_buff.push_back(p);
		pSet->Next();
	}
	pSet->Close();
	delete pSet;

	return true;
}


bool read_dzfx_policy(Database* szx_db, char* sql)
{
	if (g_dzfx_buff.size() > 0)
	{
		list<DzfxPolicy*>::iterator iter;
		for(iter = g_dzfx_buff.begin(); iter != g_dzfx_buff.end(); iter++)
			delete *iter;
		g_dzfx_buff.clear();
	}
	Recordset* pSet = new Recordset(szx_db);
	if(!pSet->Open(sql))
		return false;

	while (pSet->IsEOF() == false)
	{
		DzfxPolicy p;
		p.zbid = pSet->GetIntValue(1);
		p.zxid = pSet->GetIntValue(2);
		string tmp;
		tmp = pSet->GetStringValue(3);
		strcpy(p.dz1, tmp.c_str());
		tmp = pSet->GetStringValue(4);
		strcpy(p.dz2, tmp.c_str());
		p.dzms = (short)pSet->GetIntValue(5);

		DzfxPolicy* pp = new DzfxPolicy(p);
		g_dzfx_buff.push_back(pp);
		pSet->Next();
	}
	pSet->Close();
	delete pSet;

	return true;
}

/*
原型	bool read_nrjk_policy(Connection* pConn, char* sql)
功能	从数据库读取地址放行策略
参数	pConn 与数据库的连接标志指针， sql 读取策略的sql语句指针 
返回值  true 成功， false 失败
*/
bool read_dzfd_policy(Database* szx_db, char* sql)
{
	if (g_dzfd_buff.size() > 0)
	{
		list<DzfdPolicy*>::iterator iter;
		for(iter = g_dzfd_buff.begin(); iter != g_dzfd_buff.end(); iter++)
			delete *iter;
		g_dzfd_buff.clear();
	}

	Recordset* pSet = new Recordset(szx_db);
	if(!pSet->Open(sql))
		return false;

	while (pSet->IsEOF() == false)
	{
		DzfdPolicy p;
		p.zbid = pSet->GetIntValue(1);
		p.zxid = pSet->GetIntValue(2);
		string tmp;
		tmp = pSet->GetStringValue(3);
		strcpy(p.dz1, tmp.c_str());
		tmp = pSet->GetStringValue(4);
		strcpy(p.dz2, tmp.c_str());
		p.dzms = (short)pSet->GetIntValue(5);
		tmp = pSet->GetStringValue(6);
		strcpy(p.nrpz, tmp.c_str());

		DzfdPolicy* pp = new DzfdPolicy(p);
		g_dzfd_buff.push_back(pp);
		pSet->Next();
	}
	pSet->Close();
	delete pSet;

	return true;
}


/*
原型	bool read_nrjk_policy(Connection* pConn, char* sql)
功能	从数据库读取地址监控策略
参数	pConn 与数据库的连接标志指针， sql 读取策略的sql语句指针 
返回值  true 成功， false 失败
*/
bool read_dzjk_policy(Database* szx_db, char* sql)
{
	if (g_dzjk_buff.size() > 0)
	{
		list<DzjkPolicy*>::iterator iter;
		for(iter = g_dzjk_buff.begin(); iter != g_dzjk_buff.end(); iter++)
			delete *iter;
		g_dzjk_buff.clear();
	}

	Recordset* pSet = new Recordset(szx_db);
	if(!pSet->Open(sql))
		return false;

	while(pSet->IsEOF() == false)
	{
		DzjkPolicy* p = new DzjkPolicy();
		p->pzid = pSet->GetIntValue(1);
		p->zxid = pSet->GetIntValue(2);
		string tmp;
		tmp = pSet->GetStringValue(3);
		strcpy(p->dz1, tmp.c_str());
		tmp = pSet->GetStringValue(4);
		strcpy(p->dz2, tmp.c_str());
		p->dzms = (short)pSet->GetIntValue(5);
		g_dzjk_buff.push_back(p);
		pSet->Next();
	}
	pSet->Close();
	delete pSet;


	return true;
}

/*
原型	bool read_nrjk_policy(Connection* pConn, char* sql)
功能	从数据库读取内容监控策略
参数	pConn 与数据库的连接标志指针 ， sql 读取策略的sql语句指针  
返回值  true 成功， false 失败
*/
bool read_nrjk_policy(Database* szx_db, char* sql)
{
	if(g_nrjk_buff.size() > 0)
	{
		list<NrjkPolicy*>::iterator iter;
		for(iter = g_nrjk_buff.begin(); iter != g_nrjk_buff.end(); iter++)
			delete *iter;
		g_nrjk_buff.clear();
	}
	Recordset* pSet = new Recordset(szx_db);
	if(!pSet->Open(sql))
		return false;

	while(pSet->IsEOF() == false)
	{
		NrjkPolicy* p = new NrjkPolicy();
		p->zbid = pSet->GetIntValue(1);
		p->zxid = pSet->GetIntValue(2);
		string tmp;
		tmp = pSet->GetStringValue(3);
		strcpy(p->pznr, tmp.c_str());

		g_nrjk_buff.push_back(p);
		pSet->Next();
	}
	pSet->Close();
	delete pSet;

	return true;
}

/*
原型	bool get_policy_info(Database* fzx_db, TaskInfo* pInfo)
功能	生成读取策略略的sql语句
参数	pInfo 策略任务指针， fzx_db 与数据库的连接标志 
返回值  true 成功， false 失败
*/
bool get_policy_info(Database* fzx_db, TaskInfo* pInfo)
{
	char sql[512];
	switch(pInfo->pzlx)
	{
	case CLZL_GLZF:
		sprintf(sql, "select id,value,bz from dmb_glzf ");
		return read_glzf_policy(fzx_db, sql);
	case CLZL_DZBH:
		sprintf(sql, "select id,zx_id,dz1,dz2,dzms from gjzx_dzbh_zb where zt = 1 and (substr(sxd, %d, 1) = '1' or substr(sxd, 32, 1) = '1')",g_fzx_id);
		return read_dzfx_policy(fzx_db, sql);
	case CLZL_DZFX:
		sprintf(sql, "select id,zx_id,dz1,dz2,dzms from gjzx_dzfx_zb where zt = 1 and (substr(sxd, %d, 1) = '1' or substr(sxd, 32, 1) = '1')",g_fzx_id);
		return read_dzfx_policy(fzx_db, sql);
	case CLZL_DZJS:
		sprintf(sql, "select id,zx_id,dz1,dz2,dzms from gjzx_dzjs_zb where zt in (1,5) and (substr(sxd, %d, 1) = '1' or substr(sxd, 32, 1) = '1') union select id,zx_id,dz1,dz2,dzms from fzx_dzjs_zb where zt = 1 ",g_fzx_id);
		return read_dzjk_policy(fzx_db, sql);
	case CLZL_DZFD:
		sprintf(sql, "select id,zx_id,dz1,dz2,dzms,nrpz from gjzx_dzfd_zb where zt = 1 and (substr(sxd, %d, 1) = '1' or substr(sxd, 32, 1) = '1')",g_fzx_id);
		return read_dzfd_policy(fzx_db, sql);
	case CLZL_HHDZJC:
		sprintf(sql, "select id,zx_id,dz1,dz2,dzms from gjhh_dzjc_zb where zt = 1 and (substr(sxd, %d, 1) = '1' or substr(sxd, 32, 1) = '1')",g_fzx_id);
		return read_dzjk_policy(fzx_db, sql);
	case CLZL_NRJS:
		sprintf(sql, "select id,zx_id,pznr from gjzx_nrjs_zb where zt in (1,5) and (substr(sxd, %d, 1) = '1' or substr(sxd, 32, 1) = '1') union select id, zx_id, pznr from fzx_nrjs_zb where zt = 1 ",g_fzx_id);
		return read_nrjk_policy(fzx_db, sql);
	case CLZL_NRFD:
		sprintf(sql, "select id,zx_id,pznr from gjzx_nrfd_zb where zt in (1,2,5) and (substr(sxd, %d, 1) = '1' or substr(sxd, 32, 1) = '1') union select id, zx_id, pznr from fzx_nrfd_zb where zt in (1,2) ",g_fzx_id);
		return read_nrjk_policy(fzx_db, sql);
	case CLZL_NRJC:
		sprintf(sql, "select id,zx_id,pznr from gjzx_nrjc_zb where zt in (1,4,5) and (substr(sxd, %d, 1) = '1' or substr(sxd, 32, 1) = '1') union select id, zx_id, pznr from fzx_nrjc_zb where zt in (1,4) ",g_fzx_id);
		return read_nrjk_policy(fzx_db, sql);
	case CLZL_HHNRJC:
		sprintf(sql, "select id,zx_id,pznr from gjhh_nrjc_zb where zt in (1,4,5) and (substr(sxd, %d, 1) = '1' or substr(sxd, 32, 1) = '1')",g_fzx_id);
		return read_nrjk_policy(fzx_db, sql);
	default:
		return false;
	}

	return false;
}

/*
原型	bool write_send_result_back(Connection* pConn, TaskInfo* pInfo)
功能	根据任务中心ID将任务的执行情况写入国家数据库或分中心数据库
参数	pInfo 策略任务指针， pConn 与数据库的连接标志 
返回值  true 成功， false 失败
*/
bool write_send_result_back(Database* gjzx_db, TaskInfo* pInfo)
{
	char sql[512];
	int result = 0;

	for (int i = 0; i < g_task_target_count; i ++)
		if (policy_send_result[i] == 1)
			result++;
	if(pInfo->zxid == 0)
	{
		if (result == g_task_target_count)
			sprintf(sql, "update pzxf_rwzb_zxqk set zt = %d , wcsj = sysdate where fk_zxid = %d and zxbh = %d and zxlx = %d", 1, g_fzx_id, pInfo->rwid, 3);
		else
			sprintf(sql, "update pzxf_rwzb_zxqk set zt = %d , wcsj = sysdate where fk_zxid = %d and zxbh = %d and zxlx = %d", 2, g_fzx_id, pInfo->rwid, 3);
	}
	else
	{
		if (result == g_task_target_count)
			sprintf(sql, "update pzxf_rwzb_zxqk set zt = %d , wcsj = sysdate where zxid = %d and zxbh = %d and zxlx = %d", 1, g_fzx_id, pInfo->rwid, 3);
		else
			sprintf(sql, "update pzxf_rwzb_zxqk set zt = %d , wcsj = sysdate where zxid = %d and zxbh = %d and zxlx = %d", 2, g_fzx_id, pInfo->rwid, 3);
	}

	gjzx_db->Begin();
	if(gjzx_db->ExecUpdate(sql) == false)
	{
		gjzx_db->Rollback();
		return false;
	}
	gjzx_db->Commit();

	// 将失败明细写回通知者配置主库
	if (result < g_task_target_count)
	{
		vector<TargetInfo*>* pList;
		switch(pInfo->pzlx)
		{
		case CLZL_DZBH:		pList = &g_dzbh_targets; break;
		case CLZL_DZFX:		pList = &g_dzfx_targets; break;
		case CLZL_DZJS:		pList = &g_dzjs_targets; break;
		case CLZL_DZFD:		pList = &g_dzfd_targets; break;
		case CLZL_HHDZJC:	pList = &g_hhdzjc_targets; break;
		case CLZL_NRJS:		pList = &g_nrjs_targets; break;
		case CLZL_NRFD:		pList = &g_nrfd_targets; break;
		case CLZL_NRJC:		pList = &g_nrjc_targets; break;
		case CLZL_HHNRJC:	pList = &g_hhnrjc_targets; break;
		case CLZL_GLZF:		pList = &g_glzf_targets; break;
		default:			pList = NULL;			 break;
		}

		for (int i = 0; i < g_task_target_count; i ++)
		{

			if (policy_send_result[i] == -1)
			{
				memset(sql, 0, sizeof(sql));
				if(pInfo->zxid == 0)
				{
					sprintf(sql, "insert into pzxf_rwzb_zxqk_sbmx(zxbh, zxid, zxlx, fk_zxid, fqip, jsip, sj) values(%d, %d, %d, %d, '%s', '%s', sysdate)", pInfo->rwid, pInfo->zxid, 3, g_fzx_id, LocalIP, (*pList)[i]->ip);
				}
				else
				{
					sprintf(sql, "insert into pzxf_rwzb_zxqk_sbmx(zxbh, zxid, zxlx, fqip, jsip, sj) values(%d, %d, %d, '%s', '%s', sysdate)", pInfo->rwid, g_fzx_id, 3, LocalIP, (*pList)[i]->ip);
				}
				gjzx_db->Begin();
				if(gjzx_db->ExecUpdate(sql) == false)
				{
					gjzx_db->Rollback();
					return false;
				}
				gjzx_db->Commit();
			}						

		}
	}
	return true;

}


void* task_manage(void* p)
{
	while(1)
	{
		trapmsg_normal_fmt("###############################################################");
		trapmsg_normal_fmt("任务管理模块 开始新一轮任务监测...");
		TaskInfo* pInfo = NULL;
		pthread_mutex_lock(&task_mutex);
		if (g_task_list.size() > 0)
		{
			pInfo = g_task_list.front();
			g_task_list.pop_front();
		}
		pthread_mutex_unlock(&task_mutex);

		if (pInfo == NULL)
		{
			trapmsg_normal_fmt("本轮任务管理完成,没有可以执行的任务! %d秒后继续执行!", g_check_interval);
			trapmsg_normal_fmt("###############################################################");
			sleep(g_check_interval);
			continue;
		}
		
		if (pInfo->zxid != 0 && pInfo->zxid != g_fzx_id)
		{
			trapmsg_normal_fmt("接收到来自中心%d的非法任务通知，该任务被拒绝执行！", pInfo->zxid);
			delete pInfo;
			continue;
		}
		trapmsg_normal_fmt("任务管理模块取得了任务,任务的中心id%2d, 任务的任务id%2d, 任务的配置类型%2d !", pInfo->zxid, pInfo->rwid, pInfo->pzlx);

		Database* szx_db = NULL;

		if(gjzx_switch_yz == 0)
			szx_db = get_pzdb(&gjyd, g_fzx_id, gjzx_switch_yz, 0);			
		else
			szx_db = get_pzdb(&gjzb, g_fzx_id, gjzx_switch_yz, 0);

		if(szx_db == NULL)
		{
			szx_db = get_pzdb(&fzx1, &fzx2, g_fzx_id, 1);
		}
		if(szx_db == NULL)
		{
			pthread_mutex_lock(&task_mutex);
			g_task_list.push_back(pInfo);
			pthread_mutex_unlock(&task_mutex);
			trapmsg_normal_fmt("分中心配置主库连接获取失败，策略分发暂停！");
			sleep(g_check_interval);
			continue;
		}	
		trapmsg_normal_fmt("分中心配置主库连接获取成功！");

		if (!read_version(szx_db, pInfo->pzlx) || !get_policy_info(szx_db, pInfo))
		{
			trapmsg_normal_fmt("分中心配置主库策略获取失败，策略分发暂停！");
			delete pInfo;
			continue;
		}
		trapmsg_normal_fmt("任务管理模块模块,读取策略数据成功,准备发送！");

		// 策略版本、策略数据读取成功，准备启动线程进行分发
		g_task_finish_count = 0;
		switch(pInfo->pzlx)
		{
		case CLZL_DZBH:		g_task_target_count = g_dzbh_targets.size(); break;
		case CLZL_DZFX:		g_task_target_count = g_dzfx_targets.size(); break;
		case CLZL_DZJS:		g_task_target_count = g_dzjs_targets.size(); break;
		case CLZL_DZFD:		g_task_target_count = g_dzfd_targets.size(); break;
		case CLZL_HHDZJC:	g_task_target_count = g_hhdzjc_targets.size(); break;
		case CLZL_NRJS:		g_task_target_count = g_nrjs_targets.size(); break;
		case CLZL_NRFD:		g_task_target_count = g_nrfd_targets.size(); break;
		case CLZL_NRJC:		g_task_target_count = g_nrjc_targets.size(); break;
		case CLZL_HHNRJC:	g_task_target_count = g_hhnrjc_targets.size(); break;
		case CLZL_GLZF:		g_task_target_count = g_glzf_targets.size(); break;
		default:			g_task_target_count = 0;					 break;
		}
		if (policy_send_result)
		{
			delete []policy_send_result;
			policy_send_result = NULL;
		}
		policy_send_result = new int[g_task_target_count];
		memset(policy_send_result, 0, sizeof(policy_send_result));
		g_policy_type = pInfo->pzlx;	

		// 启动线程
		pthread_t* tid = new pthread_t[g_task_target_count];
		for(int i = 0; i< g_task_target_count; i++)
		{
			tid[i] = 0;
		}
		for(int i = 0; i < g_task_target_count; i++)
		{
			int * pint = new int(i);
			pthread_create(&tid[i] , NULL, policy_send, pint);
		}
	
		// 等待线程结束
		pthread_mutex_lock(&mutex_dispatch);
		if(g_task_finish_count < g_task_target_count)
			pthread_cond_wait(&cond_dispatch, &mutex_dispatch);
		pthread_mutex_unlock(&mutex_dispatch);
		// 处理结果
		for(int i = 0; i < g_task_target_count; i++)
		{
			pthread_join(tid[i], NULL);
		}
		delete []tid;
		
		if(pInfo->zxid == 0)
		{
			Database* gjzx_db = NULL;

			if(gjzx_switch_yz == 0)
				gjzx_db = get_pzdb(&gjyd, g_fzx_id, gjzx_switch_yz, 1);			
			else
				gjzx_db = get_pzdb(&gjzb, g_fzx_id, gjzx_switch_yz, 1);

			if(gjzx_db == NULL)
			{
				trapmsg_normal_fmt("任务执行情况回写失败！");
				//gjzx_db->Close();
				delete gjzx_db;
			}
			else
			{
				if(false == (write_send_result_back(gjzx_db, pInfo)))
					trapmsg_normal_fmt("任务执行情况回写失败！");
				gjzx_db->Close();
				delete gjzx_db;
			}
		}
		else
		{	
			if(false == (write_send_result_back(szx_db, pInfo)))
			trapmsg_normal_fmt("任务执行情况回写失败！");
		}

		szx_db->Close();
		delete szx_db;

		delete pInfo;
		trapmsg_normal_fmt("本轮任务管理完成!");
		trapmsg_normal_fmt("###############################################################");
	}

	return (void*)0;
}

