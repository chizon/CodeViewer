#ifndef __TASK_MANAGE_H__
#define __TASK_MANAGE_H__

// 策略分发目标信息
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

// 待发送策略的版本信息
extern char g_version[100];
extern int  g_policy_type;

// 链路探测时间
extern int g_link_test_time;

// 待发送的策略信息
extern list<DzfxPolicy*> g_dzfx_buff;
extern list<DzfdPolicy*> g_dzfd_buff;
extern list<DzjkPolicy*> g_dzjk_buff;
extern list<NrjkPolicy*> g_nrjk_buff;
extern list<GlzfPolicy*> g_glzf_buff;

// 策略分发线程相关
extern int  g_task_target_count;
extern int  g_task_finish_count;
extern pthread_mutex_t mutex_dispatch;
extern pthread_cond_t  cond_dispatch;

// 策略分发线程执行结果
extern int* policy_send_result;
bool task_manage_init(ArgFile* pFile);
void* task_manage(void* p);


#endif
