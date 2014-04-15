#ifndef __TASK_RECEIVE_H__
#define __TASK_RECEIVE_H__

extern list<TaskInfo*> g_task_list;
extern pthread_mutex_t task_mutex;
extern int g_read_timeout;
extern int g_write_timeout;
bool task_receive_init(ArgFile* pFile);
void* task_receive(void* p);

#endif // __TASK_RECEIVE_H__

