#ifndef __TASK_MANAGE_H__
#define __TASK_MANAGE_H__

// ���Էַ�Ŀ����Ϣ
extern vector<TargetInfo*> g_dzjs_targets;
extern vector<TargetInfo*> g_nrjs_targets;
extern vector<TargetInfo*> g_dzfx_targets;
extern vector<TargetInfo*> g_dzfd_targets;
extern vector<TargetInfo*> g_nrfd_targets;
extern vector<TargetInfo*> g_nrjc_targets;
extern vector<TargetInfo*> g_hhdzjc_targets;
extern vector<TargetInfo*> g_hhnrjc_targets;
extern vector<TargetInfo*> g_glzf_targets;
extern vector<TargetInfo*> g_dzbh_targets;

// �����Ͳ��Եİ汾��Ϣ
extern char g_version[100];
extern int  g_policy_type;

// ��·̽��ʱ��
extern int g_link_test_time;

// �����͵Ĳ�����Ϣ
extern list<DzfxPolicy*> g_dzfx_buff;
extern list<DzfdPolicy*> g_dzfd_buff;
extern list<DzjkPolicy*> g_dzjk_buff;
extern list<NrjkPolicy*> g_nrjk_buff;
extern list<GlzfPolicy*> g_glzf_buff;

// ���Էַ��߳����
extern int  g_task_target_count;
extern int  g_task_finish_count;
extern pthread_mutex_t mutex_dispatch;
extern pthread_cond_t  cond_dispatch;

// ���Էַ��߳�ִ�н��
extern int* policy_send_result;
bool task_manage_init(ArgFile* pFile);
void* task_manage(void* p);


#endif
