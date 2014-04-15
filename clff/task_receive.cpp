#include "header.h"
#include "utils.h"

#define  PACK_TPYE_CLRW_START             9001  // ��������ַ�֪ͨ 				 
#define  PACK_TPYE_CLRW_START_ANSWER      9002  // ��������ַ�֪ͨӦ��

list<TaskInfo*> g_task_list;
pthread_mutex_t task_mutex;

int g_listen_port;
int g_read_timeout;
int g_write_timeout;

bool task_receive_init(ArgFile* pFile)
{
	struct ARG* pArg;
	// �Ӵ򿪵������ļ��г�ʼ������ִ���߳���ѯʱ����
	if(pArg = pFile->GetArg("TASK_RECEIVER", "LISTEN_PORT"))
		g_listen_port= atoi(pArg->argvalue);
	else
		return false;

	if(pArg = pFile->GetArg("TASK_RECEIVER", "READ_TIMEOUT"))
		g_read_timeout= atoi(pArg->argvalue);
	else
		return false;
	if(pArg = pFile->GetArg("TASK_RECEIVER", "WRITE_TIMEOUT"))
		g_write_timeout= atoi(pArg->argvalue);
	else
		return false;

	pthread_mutex_init(&task_mutex, NULL);

	return true;
}

int	time_out(int sockfd, int sec, int usec, int waitmode)
{
	int		l_ret = 1;
	if( sockfd < 0) return -1;
	////////
	// ��ʱ�趨
	if( sec > 0 || usec >0 )
	{
		fd_set				n_fdSet;
		struct timeval		n_timeOut, n_timeLeft, n_time_start, n_time_end;

		FD_ZERO(&n_fdSet);
		FD_SET(sockfd, &n_fdSet);

		//modify by caop 03-11-12
		n_timeLeft.tv_sec = sec;
		n_timeLeft.tv_usec = usec;		
		while(1)
		{
			gettimeofday(&n_time_start, NULL);
			n_timeOut = n_timeLeft;

			if(n_timeOut.tv_sec < 0 || (n_timeOut.tv_sec == 0 && n_timeOut.tv_usec <=0)) 
			{
				l_ret = 0;	//������ʱ����
				break;
			}

			if(waitmode == READ_TIMEOUT)	// ���ö���ʱ
				l_ret = select(sockfd + 1, &n_fdSet, NULL, NULL, &n_timeOut);
			else	// ����д��ʱ
				l_ret = select(sockfd + 1, NULL, &n_fdSet, NULL, &n_timeOut);

			if( l_ret > 0 )
			{
				 if( !FD_ISSET(sockfd, &n_fdSet) )
				 {
					 l_ret = -1;					 
				 }
				 else
				 {
					 int l_error = 0;
					 socklen_t   l_len = sizeof(l_error);

					 // external code
					 if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &l_error, &l_len) < 0 || l_error != 0)
					 {
						if( l_error != 0 ) errno = l_error;
						return (-1);			// Solaris pending error
					 }
					 //
				 }
				 break;
			}
			else if( l_ret < 0 )
			{
				if( EINTR != errno )
				{
					l_ret = -1;
					break;
				}
				else
				{
					//���жϣ����㱾��select���ĵ�ʱ�䣬���ܵĳ�ʱ�����м�ȥ���´γ�ʱ����Ϊʣ���ʱ��
					gettimeofday(&n_time_end, NULL);
					n_timeLeft.tv_sec -= (n_time_end.tv_sec - n_time_start.tv_sec);
					n_timeLeft.tv_usec -= (n_time_end.tv_usec - n_time_start.tv_usec); 
					if(n_timeLeft.tv_usec < 0) 
					{
						n_timeLeft.tv_sec -= 1; 
						n_timeLeft.tv_usec += 1000000;
					}	
					continue;
				}
			}
			else
			{
				errno = ETIMEDOUT;
				break;
			}
		}// while end
	}// if end
	return l_ret;
}
/*
ԭ��	bool task_receive_taskinfo(struct TaskInfo *pTaskInfo,int sockfd)
����	���ղ��Էַ�֪ͨ��Ϣ
����	pTaskInfo ��������ָ�룬sockfd ������·��ӿڵ��������ӱ�־ 
����ֵ  true �ɹ��� false ʧ��
*/
bool task_receive_taskinfo(struct TaskInfo *pTaskInfo,int sockfd)
{
	unsigned short type;
	unsigned short len;
	char buffer[128];

	memset(pTaskInfo, 0, sizeof(TaskInfo));	

	memset(buffer, 0, sizeof(buffer));	
	if(4 != read_n(sockfd, buffer, 4))
	{
		return false;
	}
	memcpy(&len, buffer, 2);
	memcpy(&type, buffer + 2, 2);
	len  = ntohs(len);
	type = htons(type);
	if(type != PACK_TPYE_CLRW_START)		
	{
		return false;
	}

	memset(buffer, 0, sizeof(buffer));
	if(len != read_n(sockfd, buffer, len))
	{
		return false;

	}
	char* p = buffer;
	memcpy(&(pTaskInfo->zxid), p, 4);
	p += 4;
	memcpy(&(pTaskInfo->rwid), p, 4);
	p += 4;
	memcpy(&(pTaskInfo->pzlx), p, 4);
	p += 4;
	memcpy(&(pTaskInfo->zdsj), p, 4);	
	pTaskInfo->zxid = ntohl(pTaskInfo->zxid);
	pTaskInfo->rwid = ntohl(pTaskInfo->rwid);
	pTaskInfo->pzlx = ntohl(pTaskInfo->pzlx);
	pTaskInfo->zdsj = ntohl(pTaskInfo->zdsj);
	return true;
}

/*
ԭ��	bool task_receiver_ack(int sockfd)
����	���Ͳ��Էַ�֪ͨӦ����Ϣ
����	sockfd ������·��ӿڵ��������ӱ�־ 
����ֵ  true �ɹ��� false ʧ��
*/
bool task_receiver_ack(int sockfd)
{
	char buffer[4];
	unsigned short len  = htons(0);
	unsigned short type = htons(PACK_TPYE_CLRW_START_ANSWER);

	memset(buffer, 0, sizeof(buffer));
	memcpy(buffer, &len, 2);
	memcpy(buffer + 2, &type, 2);
	if( 4 != write_n(sockfd, buffer, 4))
	{
	return false;
	}
	return true;
}

/*
ԭ��	void* task_receive(void* p)
����	��������������������߳���ʽ����
*/
void* task_receive(void* p)
{   
	int listen_sockfd;
	int server_sockfd;
    struct sockaddr_in addr_local, addr_rom;
	int len_addr = sizeof(struct sockaddr);
	TaskInfo *ptaskInfo;

	memset(&addr_local, 0, sizeof(addr_local));
    addr_local.sin_family =AF_INET;
    addr_local.sin_port = htons(g_listen_port);
    addr_local.sin_addr.s_addr = htonl(INADDR_ANY);

    if((listen_sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
		trapmsg_error_fmt(CENO_SOCKET_CREAT_ERR,"SOCKET��������\n");
		return (void*)-1;
    }


	int on = 1;
	setsockopt (listen_sockfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)); 

	if(bind(listen_sockfd,(struct sockaddr*)&addr_local,sizeof(struct sockaddr))<0)
    {
		trapmsg_normal_fmt("SOCKET ������������߳�bind����ʧ�ܣ�");
		close(listen_sockfd);
		return (void*)-1;
    }
    if(listen(listen_sockfd,3) < 0)
    {
		trapmsg_normal_fmt("SOCKET ������������߳�listen����ʧ�ܣ�");
		close(listen_sockfd);
		return (void*)-1;
    }
	while(1)
	{
		if (time_out(listen_sockfd, g_read_timeout, 0, READ_TIMEOUT) == 0)
		{
			trapmsg_normal_fmt("��������̵߳ȴ�����30�볬ʱ��");
			continue;
		}
		memset(&addr_local, 0, sizeof(addr_rom));
		if((server_sockfd = accept(listen_sockfd, (struct sockaddr*)&addr_rom, (socklen_t*)&len_addr)) < 0)
		{
			trapmsg_normal_fmt("SOCKET ������������߳�accept����ʧ�ܣ�");
			continue;
		}

		ptaskInfo = new TaskInfo();
		if(false == task_receive_taskinfo(ptaskInfo, server_sockfd))
		{
			trapmsg_normal_fmt("��������߳̽�������֪ͨʧ�ܣ�");
			close(server_sockfd);
			delete ptaskInfo;
			continue;
		}

		if(false == task_receiver_ack(server_sockfd))
		{
			trapmsg_normal_fmt("��������̷߳�������֪ͨӦ��ʧ�ܣ�");
			close(server_sockfd);
			delete ptaskInfo;
			continue;
		}

		close(server_sockfd);

		trapmsg_normal_fmt("�������ģ���������֪ͨ�ɹ���  ����ID: %d  ����ID:  %d  ��������: %d  �ƶ�ʱ��: %d", ptaskInfo->zxid, ptaskInfo->rwid, ptaskInfo->pzlx, ptaskInfo->zdsj);
		pthread_mutex_lock(&task_mutex);
		g_task_list.push_back(ptaskInfo);
		pthread_mutex_unlock(&task_mutex);

	}	
	return (void*)0;
}

