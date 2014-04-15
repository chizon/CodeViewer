#include "header.h"
#include "task_receive.h"
#include "task_manage.h"
#include "Database.h"


int daemon_init()
{
	pid_t pid;
	if ((pid = fork ()) < 0)
		return -1;
	else if (pid != 0)
		exit (0);

	setsid ();
	chdir(".");

	return 0;
}

int main(int argc, char *argv[])
{
	if(argc == 2)
	{
		if(strcmp(argv[1], "-v") == 0)
		{
			printf("szx_clff �汾 2-201103221516\n");
			exit(0);
		}
	}

	daemon_init();

	signal(SIGPIPE, SIG_IGN);      // ����pipe break
	signal(SIGINT,  SIG_IGN);      // ����interrupt
	signal(SIGCHLD, SIG_IGN);
	trapmsg_init(getenv("CONFIG_FILE_PATH"), getenv("APP_IDENTIFIER"));			//trpmsg_init();

	trapmsg_normal_fmt("* * * * * * * * * * * * * * * * * * * * * * * *");
	trapmsg_normal_fmt("*                                             *");
	trapmsg_normal_fmt("*             ���Էַ���ϵͳ��������          *");
	trapmsg_normal_fmt("*                                             *");
	trapmsg_normal_fmt("* * * * * * * * * * * * * * * * * * * * * * * *");
	
	//�����ļ�ָ��
	ArgFile* pFile = new ArgFile();
	pFile->Create();
	int isok = pFile->Open("../etc/szx_clff.cfg", 0);
	if (isok != 0)
	{
		trapmsg_error_fmt(CENO_FILE_OPEN_ERROR,"�ļ���ʧ�� �ļ���:%s\n","szx_clff.cfg");

		return -1;
	}
	
	//��ʼ������ģ��
	if (!task_receive_init(pFile))
	{
		trapmsg_error_fmt(CENO_MAIN_FAILED_INIT_LISTEN,"��ʼ���������ģ��ʧ��");
		return -1;
	}
	if (!task_manage_init(pFile))
	{
		trapmsg_error_fmt(CENO_MAIN_FAILED_INIT_EXEC,"��ʼ������ִ��ģ��ʧ��");
		return -1;
	}
	
	pFile->Close();

	//��ʼ�����ݿ⻷��
	Database::InitDatabase(ora_server);

#ifdef __YD_INTERFACE__
	Database::InitDatabase(yd_server);
#endif
	//�����߳�
	//��������߳�
	pthread_t tid_task_manager;
	pthread_create(&tid_task_manager, NULL, task_manage, NULL);
	//��������߳�

	pthread_t tid_task_receive;
	pthread_create(&tid_task_receive, NULL, task_receive, NULL);

	pthread_join(tid_task_manager, NULL);
	pthread_join(tid_task_receive, NULL);
	trapmsg_stop_fmt("%s", "���Էַ���ϵͳ�˳�!");

	return 0;
}

