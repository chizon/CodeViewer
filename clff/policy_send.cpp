#include "header.h"
#include "utils.h"
#include "task_manage.h"

/*
ԭ��	int policy_send_connect(char* ip, unsigned short port)
����	��������ϵͳ������
����	ip Ҫ���ӵļ��ϵͳ���Խӿ�IP��port Ҫ���ӵļ��ϵͳ���Խӿڶ˿� 
����ֵ  �ɹ� ��������ϵͳ���Խӿڵ����ӱ�ʶ ��ʧ�� 0 
*/
int policy_send_connect(char* ip, unsigned short port)
{
	int connect_sock;

	sockaddr_in m_sockaddr;
	memset(&m_sockaddr,0, sizeof(m_sockaddr));
	m_sockaddr.sin_family =AF_INET;
	m_sockaddr.sin_port = htons(port);
	inet_aton(ip, &m_sockaddr.sin_addr);

	connect_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(connect_sock > 0)
    {	
		int l_ret = connect(connect_sock, (struct sockaddr *)&m_sockaddr, sizeof(struct sockaddr));
		if (l_ret < 0)
		{
			close(connect_sock);
			return 0;
		}
	}
	
	return connect_sock;
}

//������ʽconnect
//����ֵ���ɹ�����socket������ֵ��ʧ�ܷ���0
int link_connect(char* ip, unsigned short port)
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		//trapmsg_error_fmt(CENO_SOCKET_CREAT_ERR, "���Ӳ��� ����SOCKETʧ�ܣ�����ϵͳ״̬��");
		return 0;
	}

	int flags = 0;
	//�����ȡ�׽��ֵı�־
	if ((flags = fcntl(sockfd, F_GETFL, 0)) < 0)
	{
		//trapmsg_error_fmt(CENO_SOCKET_CREAT_ERR, "���Ӳ��� ��ȡSOCKET״̬ʧ�ܣ�����ϵͳ״̬��");
		close(sockfd);
		return 0;
	}

	//���������׽���Ϊ������
	if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) < 0)
	{
		//trapmsg_error_fmt(CENO_SOCKET_CREAT_ERR, "���Ӳ��� ����SOCKETΪ������״̬ʧ�ܣ�����ϵͳ״̬��");
		close(sockfd);
		return 0;
	}

	struct sockaddr_in  svraddr;
	memset(&svraddr,0, sizeof(svraddr));
	svraddr.sin_family = AF_INET;
	svraddr.sin_port = htons(port);
	inet_aton(ip, &svraddr.sin_addr);
	int ret = 0;
	if (((ret = connect(sockfd, (sockaddr*)&svraddr, sizeof(svraddr))) < 0) && errno != EINPROGRESS)
	{
		//��Ϊ�׽�����ΪNONBLOCK��ͨ������£�������connect()����
		//֮ǰ�ǲ��Ὠ���ģ�������᷵��EINPROGRESS�����������
		//�κ�����������Ҫ���д�����
		//trapmsg_error_fmt(CENO_SOCKET_CONN_ERR, "���Ӳ��� �������[%s:%d]����ʧ�ܣ��������缰�Զ�Ӧ��״̬!", ip, port);
		close(sockfd);
		return 0;
	}

	if (ret == 0) //���connect()����0�������ѽ���
	{  
		//����ָ��׽�������״̬
		if (fcntl(sockfd, F_SETFL, flags) < 0)
		{
			//������
			//trapmsg_error_fmt(CENO_SOCKET_CREAT_ERR, "���Ӳ��� �ָ�SOCKETΪ����״̬ʧ�ܣ�����ϵͳ״̬��");
			close(sockfd);
			return 0;
		}

		//���������ӳɹ���Ҫִ�еĴ���
		//trapmsg_normal_fmt("���Ӳ��� ��·̽��ɹ� 1 IP[%s] PORT[%d]", ip, port);
		return sockfd;
	}


	fd_set readfds,writefds,expenfds;
	FD_ZERO(&readfds);
	FD_SET(sockfd, &readfds);  //����ǰ���׽��ּӵ�����������
	writefds = readfds;        //д����
	expenfds = readfds;        //�쳣����

	struct timeval tv;
	tv.tv_sec = g_link_test_time;  //����ʱ��Ϊ5��
	tv.tv_usec = 0;

	ret = select(sockfd+1, &readfds, &writefds, &expenfds, &tv);
	if (ret < 0)  //select���ش���
	{  
		//������
		//trapmsg_error_fmt(CENO_SOCKET_CREAT_ERR, "���Ӳ��� ϵͳ����SELECTʧ�ܣ�����ϵͳ״̬��");
		close(sockfd);
		return 0;
	}
	else if (ret == 0) //select��ʱ
	{ 
		//��ʱ����
		//trapmsg_error_fmt(CENO_SOCKET_CONN_ERR, "���Ӳ��� �������[%s:%d]���ӳ�ʱ���������缰�Զ�Ӧ��״̬!", ip, port);
		close(sockfd);
		return 0;
	}
	else
	{
		//�׽����Ѿ�׼����
		if (!FD_ISSET(sockfd, &readfds) && !FD_ISSET(sockfd, &writefds))
		{
			//connect()ʧ�ܣ����д�����
			//trapmsg_error_fmt(CENO_SOCKET_CONN_ERR, "���Ӳ��� �������[%s:%d]����ʧ�ܣ��������缰�Զ�Ӧ��״̬!", ip, port);
			close(sockfd);
			return 0;
		}

		int err = 0;
		socklen_t len;
		len = sizeof(int);
		if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &err, &len) < 0)
		{
			//getsockopt()ʧ�ܣ����д�����
			//trapmsg_error_fmt(CENO_SOCKET_CREAT_ERR, "���Ӳ��� ���SOCKET״̬ʧ�ܣ�����ϵͳ״̬��");
			close(sockfd);
			return 0;
		}

		if (err != 0)
		{
			//connect()ʧ�ܣ����д�����
			//trapmsg_error_fmt(CENO_SOCKET_CREAT_ERR, "���Ӳ��� CONNECTʧ�ܣ�err[%d]��", err);
			close(sockfd);
			return 0;
		}

		//������˵��connect()��ȷ����
		//����ָ��׽�������״̬
		if (fcntl(sockfd, F_SETFL, flags) < 0)
		{
			//������
			//trapmsg_error_fmt(CENO_SOCKET_CREAT_ERR, "���Ӳ��� �ָ�SOCKETΪ����״̬ʧ�ܣ�����ϵͳ״̬��");
			close(sockfd);
			return 0;
		}

		//���������ӳɹ���Ҫִ�еĴ���
		//trapmsg_normal_fmt("���Ӳ��� ��·̽��ɹ� 2 IP[%s] PORT[%d]", ip, port);
		return sockfd;
	}
}



/*
ԭ��	bool policy_send_notify(int connId, PolicyInfo* pInfo)
����	���Ͳ���֪ͨ��Ϣ
����	connId����policy_send_connect���÷����������ӱ�ʶ
����ֵ	true ��ʾ�ɹ���
        false ��ʾִ��ʧ�ܡ�
*/
bool policy_send_notify(int connId, PolicyInfo* pInfo)
{
	char buffer[128];
	unsigned short type;
	unsigned short len;

	memset(buffer, 0, sizeof(buffer));
	len = htons(108);
	type = htons(PACK_TPYE_CLFF_START);
	memcpy(buffer, &len, 2);
	memcpy(buffer + 2, &type, 2);

	int tmp = pInfo->type;
	tmp = htonl(tmp);
	memcpy(buffer + 4, &tmp, sizeof(int));

	switch(pInfo->type)
	{
	case CLZL_DZBH:
	case CLZL_DZFX:		tmp = g_dzfx_buff.size(); break;
	case CLZL_DZFD:		tmp = g_dzfd_buff.size(); break;
	case CLZL_DZJS:
	case CLZL_HHDZJC:	tmp = g_dzjk_buff.size(); break;
	case CLZL_NRJS:
	case CLZL_NRFD:
	case CLZL_NRJC:
	case CLZL_HHNRJC:	tmp = g_nrjk_buff.size(); break;
	case CLZL_GLZF:		tmp = g_glzf_buff.size(); break;
	default:			tmp = 0;				  break;
	}
	tmp = htonl(tmp);
	memcpy(buffer + 8, &tmp, sizeof(int));
	memcpy(buffer + 12, g_version, 100);

	return (write_n(connId, buffer, 112) == 112);
}

/*
ԭ��	bool policy_send_recv_ack(int connId)
����	���ղ���֪ͨӦ��
����	connId����policy_send_connect���÷����������ӱ�ʶ
����ֵ	true ��ʾ�ɹ���
        false ��ʾִ��ʧ�ܡ�
*/
bool policy_send_recv_ack(int connId)
{
	char buffer[4];
	unsigned short type;
	unsigned short len;

	memset(buffer, 0, sizeof(buffer));
	if (read_n(connId, buffer, sizeof(buffer)) <= 0)
		return false;
	len = *(short*)buffer;
	type = *(short*)(buffer + 2);
	len = ntohs(len);
	type = ntohs(type);

	if (type != PACK_TPYE_CLFF_START_ANSWER)
		return false;
	else
		return true;
}

/*
ԭ��	bool policy_send_policy(int connId, int cltype, struct DzfxPolicy *pPolicy)
����	���͵�ַ���в���
����	connId����policy_send_connect���÷����������ӱ�ʶ
����ֵ	true ��ʾ�ɹ���
        false ��ʾִ��ʧ�ܡ�
*/
bool policy_send_policy(int connId, int cltype, struct DzfxPolicy *pPolicy)
{
	unsigned short type;
	unsigned short body_len;
	int total_len;
	char data[512];
	char* p = data + 4;

	switch(cltype)
	{
	// ����ϵͳ
	case CLZL_DZJS:
		type = htons(PACK_TPYE_DZJS);
		break;
	case CLZL_NRJS:
		type = htons(PACK_TPYE_NRJS);
		break;
	//���ϵͳ
	case CLZL_DZBH:
		type = htons(PACK_TPYE_DZBH);
		break;
	case CLZL_DZFX:
		type = htons(PACK_TPYE_DZFX);
		break;
	case CLZL_DZFD:
		type = htons(PACK_TPYE_DZFD);
		break;
	case CLZL_NRFD:
		type = htons(PACK_TPYE_NRFD);
		break;
	//���
	case CLZL_NRJC:
		type = htons(PACK_TPYE_NRJC);
		break;
	//�ں�
	case CLZL_HHDZJC:
		type = htons(PACK_TPYE_HHDZJC);
		break;
	case CLZL_HHNRJC:
		type = htons(PACK_TPYE_HHNRJC);
		break;
	//�����ַ�
	case CLZL_GLZF:
		type = htons(PACK_TPYE_GLZF);
		break;
	default:
		return false;
	}

	memset(data, 0, sizeof(data));
	int tmp;
	tmp = pPolicy->zbid;
	tmp = htonl(tmp);
	memcpy(p, &tmp, 4);
	p += 4;
	tmp = pPolicy->zxid;
	tmp = htonl(tmp);
	memcpy(p, &tmp, 4);
	p += 4;

	memcpy(p, pPolicy->dz1, 30);
	p += 30;

	memcpy(p, pPolicy->dz2, 30);
	p += 30;

	tmp = pPolicy->dzms;
	tmp = htons(tmp);
	memcpy(p, &tmp, 2);
	p += 2;
	body_len = 70;
	total_len = body_len + 4;

	body_len = htons(body_len);

	memcpy(data, &body_len, 2);
	memcpy(data + 2, &type, 2);

	if (write_n(connId, data, total_len) <= 0)
		return false;
	else
		return true;
}

/*
ԭ��	bool policy_send_policy(int connId, int cltype, struct DzfxPolicy *pPolicy)
����	���͵�ַ��²���
����	connId����policy_send_connect���÷����������ӱ�ʶ
����ֵ	true ��ʾ�ɹ���
        false ��ʾִ��ʧ�ܡ�
*/
bool policy_send_policy(int connId, int cltype, struct DzfdPolicy *pPolicy)
{
	unsigned short type;
	unsigned short body_len;
	int total_len;
	char data[512];
	char* p = data + 4;

	switch(cltype)
	{
	// ����ϵͳ
	case CLZL_DZJS:
		type = htons(PACK_TPYE_DZJS);
		break;
	case CLZL_NRJS:
		type = htons(PACK_TPYE_NRJS);
		break;
	//���ϵͳ	
	case CLZL_DZBH:
		type = htons(PACK_TPYE_DZBH);
		break;
	case CLZL_DZFX:
		type = htons(PACK_TPYE_DZFX);
		break;
	case CLZL_DZFD:
		type = htons(PACK_TPYE_DZFD);
		break;
	case CLZL_NRFD:
		type = htons(PACK_TPYE_NRFD);
		break;
	//���
	case CLZL_NRJC:
		type = htons(PACK_TPYE_NRJC);
		break;
	//�ں�
	case CLZL_HHDZJC:
		type = htons(PACK_TPYE_HHDZJC);
		break;
	case CLZL_HHNRJC:
		type = htons(PACK_TPYE_HHNRJC);
		break;
	//�����ַ�
	case CLZL_GLZF:
		type = htons(PACK_TPYE_GLZF);
		break;
	default:
		return false;
	}

	memset(data, 0, sizeof(data));
	int tmp;
	tmp = pPolicy->zbid;
	tmp = htonl(tmp);
	memcpy(p, &tmp, 4);
	p += 4;
	tmp = pPolicy->zxid;
	tmp = htonl(tmp);
	memcpy(p, &tmp, 4);
	p += 4;

	memcpy(p, pPolicy->dz1, 30);
	p += 30;

	memcpy(p, pPolicy->dz2, 30);
	p += 30;

	tmp = pPolicy->dzms;
	tmp = htons(tmp);
	memcpy(p, &tmp, 2);
	p += 2;

	memcpy(p, pPolicy->nrpz, 256);
	p += 256;

	body_len = 326;
	total_len = body_len + 4;
	body_len = htons(body_len);

	memcpy(data, &body_len, 2);
	memcpy(data + 2, &type, 2);

	if (write_n(connId, data, total_len) <= 0)
		return false;
	else
		return true;
}


/*
ԭ��	bool policy_send_policy(int connId, int cltype, struct DzfxPolicy *pPolicy)
����	���͵�ַ��ز���
����	connId����policy_send_connect���÷����������ӱ�ʶ
����ֵ	true ��ʾ�ɹ���
        false ��ʾִ��ʧ�ܡ�
*/
bool policy_send_policy(int connId,  int cltype, DzjkPolicy *pPolicy)
{	
	unsigned short type;
	unsigned short body_len;
	int total_len;
	char data[512];
	char* p = data + 4;

	switch(cltype)
	{
	// ����ϵͳ
	case CLZL_DZJS:
		type = htons(PACK_TPYE_DZJS);
		break;
	case CLZL_NRJS:
		type = htons(PACK_TPYE_NRJS);
		break;
	//���ϵͳ	
	case CLZL_DZBH:
		type = htons(PACK_TPYE_DZBH);
		break;
	case CLZL_DZFX:
		type = htons(PACK_TPYE_DZFX);
		break;
	case CLZL_DZFD:
		type = htons(PACK_TPYE_DZFD);
		break;
	case CLZL_NRFD:
		type = htons(PACK_TPYE_NRFD);
		break;
	//���
	case CLZL_NRJC:
		type = htons(PACK_TPYE_NRJC);
		break;
	//�ں�
	case CLZL_HHDZJC:
		type = htons(PACK_TPYE_HHDZJC);
		break;
	case CLZL_HHNRJC:
		type = htons(PACK_TPYE_HHNRJC);
		break;
	//�����ַ�
	case CLZL_GLZF:
		type = htons(PACK_TPYE_GLZF);
		break;
	default:
		return false;
	}

	memset(data, 0, sizeof(data));
	int tmp;
	tmp = pPolicy->pzid;
	tmp = htonl(tmp);
	memcpy(p, &tmp, 4);
	p += 4;
	tmp = pPolicy->zxid;
	tmp = htonl(tmp);
	memcpy(p, &tmp, 4);
	p += 4;

	memcpy(p, pPolicy->dz1, 30);
	p += 30;

	memcpy(p, pPolicy->dz2, 30);
	p += 30;

	tmp = pPolicy->dzms;
	tmp = htons(tmp);
	memcpy(p, &tmp, 2);
	p += 2;

	body_len = 70;//Ϊ��sxwg�޸� 20090615 / 6 sunguohua
	total_len = body_len + 4;
	body_len = htons(body_len);

	memcpy(data, &body_len, 2);
	memcpy(data + 2, &type, 2);
	if (write_n(connId, data, total_len) <= 0)
		return false;
	else
		return true;
}

/*
ԭ��	bool policy_send_policy(int connId, int cltype, struct DzfxPolicy *pPolicy)
����	�������ݼ�ز���
����	connId����policy_send_connect���÷����������ӱ�ʶ
����ֵ	true ��ʾ�ɹ���
        false ��ʾִ��ʧ�ܡ�
*/
bool policy_send_policy(int connId,  int cltype, struct NrjkPolicy *pPolicy)
{
	unsigned short type;
	unsigned short body_len;
	int total_len;
	char data[512];
	char* p = data + 4;

	switch(cltype)
	{
	// ����ϵͳ
	case CLZL_DZJS:
		type = htons(PACK_TPYE_DZJS);
		break;
	case CLZL_NRJS:
		type = htons(PACK_TPYE_NRJS);
		break;
	//���ϵͳ	
	case CLZL_DZBH:
		type = htons(PACK_TPYE_DZBH);
		break;
	case CLZL_DZFX:
		type = htons(PACK_TPYE_DZFX);
		break;
	case CLZL_DZFD:
		type = htons(PACK_TPYE_DZFD);
		break;
	case CLZL_NRFD:
		type = htons(PACK_TPYE_NRFD);
		break;
	//���
	case CLZL_NRJC:
		type = htons(PACK_TPYE_NRJC);
		break;
	//�ں�
	case CLZL_HHDZJC:
		type = htons(PACK_TPYE_HHDZJC);
		break;
	case CLZL_HHNRJC:
		type = htons(PACK_TPYE_HHNRJC);
		break;
	//�����ַ�
	case CLZL_GLZF:
		type = htons(PACK_TPYE_GLZF);
		break;
	default:
		return false;
	}

	memset(data, 0, sizeof(data));
	int tmp;
	tmp = pPolicy->zbid;
	tmp = htonl(tmp);
	memcpy(p, &tmp, 4);
	p += 4;
	tmp = pPolicy->zxid;
	tmp = htonl(tmp);
	memcpy(p, &tmp, 4);
	p += 4;

	memcpy(p, pPolicy->pznr,256);
	p += 256;

	body_len = 264;
	total_len = body_len + 4;
	body_len = htons(body_len);

	memcpy(data, &body_len, 2);
	memcpy(data + 2, &type, 2);
	if (write_n(connId, data, total_len) <= 0)
		return false;
	else
		return true;
}

/*
ԭ��	bool policy_send_policy(int connId, int cltype, struct DzfxPolicy *pPolicy)
����	���͹����ַ�����
����	connId����policy_send_connect���÷����������ӱ�ʶ
����ֵ	true ��ʾ�ɹ���
        false ��ʾִ��ʧ�ܡ�
*/
bool policy_send_policy(int connId,  int cltype, struct GlzfPolicy *pPolicy)
{
	unsigned short type;
	unsigned short body_len;
	int total_len;
	char data[512];
	char* p = data + 4;

	switch(cltype)
	{
	// ����ϵͳ
	case CLZL_DZJS:
		type = htons(PACK_TPYE_DZJS);
		break;
	case CLZL_NRJS:
		type = htons(PACK_TPYE_NRJS);
		break;
	//���ϵͳ	
	case CLZL_DZBH:
		type = htons(PACK_TPYE_DZBH);
		break;
	case CLZL_DZFX:
		type = htons(PACK_TPYE_DZFX);
		break;
	case CLZL_DZFD:
		type = htons(PACK_TPYE_DZFD);
		break;
	case CLZL_NRFD:
		type = htons(PACK_TPYE_NRFD);
		break;
	//���
	case CLZL_NRJC:
		type = htons(PACK_TPYE_NRJC);
		break;
	//�ں�
	case CLZL_HHDZJC:
		type = htons(PACK_TPYE_HHDZJC);
		break;
	case CLZL_HHNRJC:
		type = htons(PACK_TPYE_HHNRJC);
		break;
	//�����ַ�
	case CLZL_GLZF:
		type = htons(PACK_TPYE_GLZF);
		break;
	default:
		return false;
	}

	memset(data, 0, sizeof(data));
	int tmp;
	tmp = pPolicy->id;
	tmp = htonl(tmp);
	memcpy(p, &tmp, 4);
	p += 4;
	memcpy(p, pPolicy->zfz,5);
	p += 5;

	memcpy(p, pPolicy->bz,4);
	p += 4;

	body_len = 13;
	total_len = body_len + 4;
	body_len = htons(body_len);

	memcpy(data, &body_len, 2);
	memcpy(data + 2, &type, 2);
	if (write_n(connId, data, total_len) <= 0)
		return false;
	else
		return true;
}


/*
ԭ��	bool policy_send_data(int connId, int cltype)
����	���Է���ѡ��
����	connId����policy_send_connect���÷����������ӱ�ʶ
		cltype�� ��������
����ֵ	true ��ʾ�ɹ���
        false ��ʾִ��ʧ�ܡ�
*/
bool policy_send_data(int connId, int cltype)
{
	switch(cltype)
	{
	case CLZL_DZBH:
	case CLZL_DZFX:
		{
			list<DzfxPolicy*>::iterator iter;
			for(iter = g_dzfx_buff.begin(); iter != g_dzfx_buff.end(); iter++)
				if (!policy_send_policy(connId, cltype, *iter))
					return false;
			return true;
		}
	case CLZL_DZFD:
		{
			list<DzfdPolicy*>::iterator iter;
			for(iter = g_dzfd_buff.begin(); iter != g_dzfd_buff.end(); iter++)
				if (!policy_send_policy(connId, cltype, *iter))
					return false;
			return true;
		}

	case CLZL_DZJS:
	case CLZL_HHDZJC:
		{
			list<DzjkPolicy*>::iterator iter;
			for(iter = g_dzjk_buff.begin(); iter != g_dzjk_buff.end(); iter++)
				if (!policy_send_policy(connId, cltype, *iter))
					return false;
			return true;
		}

	case CLZL_NRJS:
	case CLZL_NRFD:
	case CLZL_NRJC:
	case CLZL_HHNRJC:
		{
			list<NrjkPolicy*>::iterator iter;
			for(iter = g_nrjk_buff.begin(); iter != g_nrjk_buff.end(); iter++)
				if (!policy_send_policy(connId, cltype, *iter))
					return false;
			return true;
		}
	case CLZL_GLZF:
		{
			list<GlzfPolicy*>::iterator iter;
			for(iter = g_glzf_buff.begin(); iter != g_glzf_buff.end(); iter++)
				if (!policy_send_policy(connId, cltype, *iter))
					return false;
			return true;
		}

	default:
		return false;
	}

	return false;
}

/*
ԭ��	bool policy_send_finish(int connId)
����	�������Է���
����	connId����policy_send_connect���÷����������ӱ�ʶ
����ֵ	true����ʾִ�гɹ���
        flase����ʾִ�г���
*/
bool policy_send_finish(int connId)
{
	char buffer[4];
	unsigned short type;
	unsigned short len;

	len  = htons(0);
	type = htons(PACK_TPYE_CLFF_END);
	memset(buffer, 0, sizeof(buffer));
	memcpy(buffer, &len, 2);
	memcpy(buffer + 2, &type, 2);

	if (write_n(connId, buffer, 4) <= 0)
		return false;

	memset(buffer, 0, sizeof(buffer));

	if (read_n(connId, buffer, 4) <= 0)
		return false;
	else		
	{	
		memcpy(&len, buffer, 2);
		memcpy(&type, buffer + 2, 2);
		type = ntohs(type);
		if(type != PACK_TPYE_CLFF_END_ANSWER)
			return false;
		else
			return true;	
	}
}

/*
ԭ��	void policy_send_leave(int index, int result)
����	�����Է������д����Էַ��߳�ִ�н��
����	index�����Է���ģ��ID��result ����ִ�н��
*/
void policy_send_leave(int index, int result)
{
	pthread_mutex_lock(&mutex_dispatch);
	policy_send_result[index] = result;
	g_task_finish_count++;
	if (g_task_finish_count == g_task_target_count)
		pthread_cond_broadcast(&cond_dispatch);
	pthread_mutex_unlock(&mutex_dispatch);
}

/*
ԭ��	void *policy_send(void *p)
����	���Է����������������߳���ʽ����
*/
void *policy_send(void *p)
{
	int index =*(int *)p;
	delete (int *)p;

	PolicyInfo info;
	info.type = g_policy_type;
	memcpy(info.version, g_version, sizeof(g_version));

	vector<TargetInfo*>* pList;

	switch(g_policy_type)
	{
	case CLZL_DZBH:		pList = &g_dzbh_targets; info.count = g_dzfx_buff.size(); break;
	case CLZL_DZFX:		pList = &g_dzfx_targets; info.count = g_dzfx_buff.size(); break;
	case CLZL_DZJS:		pList = &g_dzjs_targets; info.count = g_dzjk_buff.size(); break;
	case CLZL_DZFD:		pList = &g_dzfd_targets; info.count = g_dzfd_buff.size(); break;
	case CLZL_HHDZJC:	pList = &g_hhdzjc_targets; info.count = g_dzjk_buff.size(); break;
	case CLZL_NRJS:		pList = &g_nrjs_targets; info.count = g_nrjk_buff.size(); break;
	case CLZL_NRFD:		pList = &g_nrfd_targets; info.count = g_nrjk_buff.size(); break;
	case CLZL_NRJC:		pList = &g_nrjc_targets; info.count = g_nrjk_buff.size(); break;
	case CLZL_HHNRJC:	pList = &g_hhnrjc_targets; info.count = g_nrjk_buff.size(); break;
	case CLZL_GLZF:		pList = &g_glzf_targets; info.count = g_glzf_buff.size(); break;
	default:			pList = NULL;			 break;
	}
	//int sockfd = policy_send_connect((*pList)[index]->ip, (*pList)[index]->port);
	int sockfd = link_connect((*pList)[index]->ip, (*pList)[index]->port);
	if (sockfd <= 0)
	{
		trapmsg_error_fmt(CENO_SOCKET_CONN_ERR,"���Էַ�ģ������ż��ϵͳ���Խ���ģ��[ip = %s, port = %d]���Ӽ��ϵͳʧ��,����Զ˵�����״̬�Ϳعܳ��������״̬!",(*pList)[index]->ip,(*pList)[index]->port);
		policy_send_leave(index, -1);
		return (void *)-1;
	}
	trapmsg_normal_fmt("���Էַ�ģ������ż��ϵͳ���Խ���ģ��[ip = %s, port = %d]���ӳɹ���",(*pList)[index]->ip,(*pList)[index]->port);

	if (!policy_send_notify(sockfd, &info))
	{	
		trapmsg_error_fmt(CENO_SOCKET_SEND_DATA_ERR,"���Էַ�ģ������ż��ϵͳ���Խ���ģ��[ip = %s, port = %d]���Ͳ��Էַ�֪ͨʧ��,����Զ˵�����״̬�Ϳعܳ��������״̬!",(*pList)[index]->ip, (*pList)[index]->port);
		close(sockfd);
		policy_send_leave(index, -1);
		return (void *)-1;
	}
	trapmsg_normal_fmt("���Էַ�ģ������ż��ϵͳ���Խ���ģ��[ip = %s, port = %d]���Ͳ��Էַ�֪ͨ�ɹ�!",(*pList)[index]->ip, (*pList)[index]->port);

	if(!policy_send_recv_ack(sockfd))
	{
		 struct sockaddr_in addr_local;
		 int len_addr = sizeof(struct sockaddr);
		 memset(&addr_local, 0, sizeof(addr_local));
		getsockname(sockfd, (struct sockaddr*)&addr_local, (socklen_t*)&len_addr);
		trapmsg_error_fmt(CENO_SOCKET_RECV_DATA_ERR,"���Էַ�ģ���ȡ���ż��ϵͳ���Խ���ģ��[ip = %s, port = %d]���Էַ�Ӧ��ʧ��,����Զ˵�����״̬�Ϳعܳ��������״̬!",(*pList)[index]->ip, (*pList)[index]->port);
		close(sockfd);
		policy_send_leave(index, -1);
		return (void *)-1;
	}
	trapmsg_normal_fmt("���Էַ�ģ���ȡ���ż��ϵͳ���Խ���ģ��[ip = %s, port = %d]���Էַ�Ӧ��ɹ���",(*pList)[index]->ip, (*pList)[index]->port);

	if(!policy_send_data(sockfd, g_policy_type))
	{
		trapmsg_error_fmt(CENO_SOCKET_SEND_DATA_ERR,"���Էַ�ģ������ż��ϵͳ���Խ���ģ��[ip = %s, port = %d]���Ͳ���ʧ��,����Զ˵�����״̬�Ϳعܳ��������״̬��",(*pList)[index]->ip, (*pList)[index]->port);
		close(sockfd);
		policy_send_leave(index, -1);
		return (void *)-1;
	}
	trapmsg_normal_fmt("���Էַ�ģ������ż��ϵͳ���Խ���ģ��[ip = %s, port = %d]���Ͳ��Գɹ�", (*pList)[index]->ip, (*pList)[index]->port);

	if(!policy_send_finish(sockfd))
	{
		trapmsg_error_fmt(CENO_SOCKET_SEND_DATA_ERR,"���Էַ�ģ������ż��ϵͳ���Խ���ģ��[ip = %s, port = %d]���Ͳ��Էַ�����֪ͨ(���ղ��Էַ�����Ӧ��)ʧ��,����Զ˵�����״̬�Ϳعܳ��������״̬��",(*pList)[index]->ip, (*pList)[index]->port);
		close(sockfd);
		policy_send_leave(index, -1);
		return (void *)-1;
	}
	trapmsg_normal_fmt("���Էַ�ģ������ż��ϵͳ���Խ���ģ��[ip = %s, port = %d]���Ͳ��Էַ�����֪ͨ(���ղ��Էַ�����Ӧ��)�ɹ���",(*pList)[index]->ip, (*pList)[index]->port);

	trapmsg_normal_fmt("���Էַ�ģ����[ip = %s, port = %d]���Ͳ���[type = %d]��Ŀ[num= %d]�ɹ���ɣ�", (*pList)[index]->ip, (*pList)[index]->port, info.type, info.count);

	close(sockfd);
	policy_send_leave(index, 1);
	return (void *)0;
}
