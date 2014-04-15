#include "header.h"
#include "utils.h"
#include "task_manage.h"

/*
原型	int policy_send_connect(char* ip, unsigned short port)
功能	建立与监控系统的连接
参数	ip 要连接的监控系统策略接口IP，port 要连接的监控系统策略接口端口 
返回值  成功 返回与监控系统策略接口的连接标识 ，失败 0 
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

//非阻塞式connect
//返回值：成功返回socket描述符值，失败返回0
int link_connect(char* ip, unsigned short port)
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		//trapmsg_error_fmt(CENO_SOCKET_CREAT_ERR, "连接测试 创建SOCKET失败，请检查系统状态！");
		return 0;
	}

	int flags = 0;
	//下面获取套接字的标志
	if ((flags = fcntl(sockfd, F_GETFL, 0)) < 0)
	{
		//trapmsg_error_fmt(CENO_SOCKET_CREAT_ERR, "连接测试 获取SOCKET状态失败，请检查系统状态！");
		close(sockfd);
		return 0;
	}

	//下面设置套接字为非阻塞
	if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) < 0)
	{
		//trapmsg_error_fmt(CENO_SOCKET_CREAT_ERR, "连接测试 设置SOCKET为非阻塞状态失败，请检查系统状态！");
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
		//因为套接字设为NONBLOCK，通常情况下，连接在connect()返回
		//之前是不会建立的，因此它会返回EINPROGRESS错误，如果返回
		//任何其他错误，则要进行错误处理
		//trapmsg_error_fmt(CENO_SOCKET_CONN_ERR, "连接测试 与服务器[%s:%d]连接失败，请检查网络及对端应用状态!", ip, port);
		close(sockfd);
		return 0;
	}

	if (ret == 0) //如果connect()返回0则连接已建立
	{  
		//下面恢复套接字阻塞状态
		if (fcntl(sockfd, F_SETFL, flags) < 0)
		{
			//错误处理
			//trapmsg_error_fmt(CENO_SOCKET_CREAT_ERR, "连接测试 恢复SOCKET为阻塞状态失败，请检查系统状态！");
			close(sockfd);
			return 0;
		}

		//下面是连接成功后要执行的代码
		//trapmsg_normal_fmt("连接测试 链路探测成功 1 IP[%s] PORT[%d]", ip, port);
		return sockfd;
	}


	fd_set readfds,writefds,expenfds;
	FD_ZERO(&readfds);
	FD_SET(sockfd, &readfds);  //把先前的套接字加到读集合里面
	writefds = readfds;        //写集合
	expenfds = readfds;        //异常集合

	struct timeval tv;
	tv.tv_sec = g_link_test_time;  //设置时间为5秒
	tv.tv_usec = 0;

	ret = select(sockfd+1, &readfds, &writefds, &expenfds, &tv);
	if (ret < 0)  //select返回错误
	{  
		//错误处理
		//trapmsg_error_fmt(CENO_SOCKET_CREAT_ERR, "连接测试 系统调用SELECT失败，请检查系统状态！");
		close(sockfd);
		return 0;
	}
	else if (ret == 0) //select超时
	{ 
		//超时处理
		//trapmsg_error_fmt(CENO_SOCKET_CONN_ERR, "连接测试 与服务器[%s:%d]连接超时，请检查网络及对端应用状态!", ip, port);
		close(sockfd);
		return 0;
	}
	else
	{
		//套接字已经准备好
		if (!FD_ISSET(sockfd, &readfds) && !FD_ISSET(sockfd, &writefds))
		{
			//connect()失败，进行错误处理
			//trapmsg_error_fmt(CENO_SOCKET_CONN_ERR, "连接测试 与服务器[%s:%d]连接失败，请检查网络及对端应用状态!", ip, port);
			close(sockfd);
			return 0;
		}

		int err = 0;
		socklen_t len;
		len = sizeof(int);
		if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &err, &len) < 0)
		{
			//getsockopt()失败，进行错误处理
			//trapmsg_error_fmt(CENO_SOCKET_CREAT_ERR, "连接测试 获得SOCKET状态失败，请检查系统状态！");
			close(sockfd);
			return 0;
		}

		if (err != 0)
		{
			//connect()失败，进行错误处理
			//trapmsg_error_fmt(CENO_SOCKET_CREAT_ERR, "连接测试 CONNECT失败，err[%d]！", err);
			close(sockfd);
			return 0;
		}

		//到这里说明connect()正确返回
		//下面恢复套接字阻塞状态
		if (fcntl(sockfd, F_SETFL, flags) < 0)
		{
			//错误处理
			//trapmsg_error_fmt(CENO_SOCKET_CREAT_ERR, "连接测试 恢复SOCKET为阻塞状态失败，请检查系统状态！");
			close(sockfd);
			return 0;
		}

		//下面是连接成功后要执行的代码
		//trapmsg_normal_fmt("连接测试 链路探测成功 2 IP[%s] PORT[%d]", ip, port);
		return sockfd;
	}
}



/*
原型	bool policy_send_notify(int connId, PolicyInfo* pInfo)
功能	发送策略通知消息
参数	connId，由policy_send_connect调用返回网络连接标识
返回值	true 表示成功，
        false 表示执行失败。
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
原型	bool policy_send_recv_ack(int connId)
功能	接收策略通知应答
参数	connId，由policy_send_connect调用返回网络连接标识
返回值	true 表示成功，
        false 表示执行失败。
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
原型	bool policy_send_policy(int connId, int cltype, struct DzfxPolicy *pPolicy)
功能	发送地址放行策略
参数	connId，由policy_send_connect调用返回网络连接标识
返回值	true 表示成功，
        false 表示执行失败。
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
	// 监视系统
	case CLZL_DZJS:
		type = htons(PACK_TPYE_DZJS);
		break;
	case CLZL_NRJS:
		type = htons(PACK_TPYE_NRJS);
		break;
	//封堵系统
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
	//监测
	case CLZL_NRJC:
		type = htons(PACK_TPYE_NRJC);
		break;
	//黑盒
	case CLZL_HHDZJC:
		type = htons(PACK_TPYE_HHDZJC);
		break;
	case CLZL_HHNRJC:
		type = htons(PACK_TPYE_HHNRJC);
		break;
	//过滤字符
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
原型	bool policy_send_policy(int connId, int cltype, struct DzfxPolicy *pPolicy)
功能	发送地址封堵策略
参数	connId，由policy_send_connect调用返回网络连接标识
返回值	true 表示成功，
        false 表示执行失败。
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
	// 监视系统
	case CLZL_DZJS:
		type = htons(PACK_TPYE_DZJS);
		break;
	case CLZL_NRJS:
		type = htons(PACK_TPYE_NRJS);
		break;
	//封堵系统	
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
	//监测
	case CLZL_NRJC:
		type = htons(PACK_TPYE_NRJC);
		break;
	//黑盒
	case CLZL_HHDZJC:
		type = htons(PACK_TPYE_HHDZJC);
		break;
	case CLZL_HHNRJC:
		type = htons(PACK_TPYE_HHNRJC);
		break;
	//过滤字符
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
原型	bool policy_send_policy(int connId, int cltype, struct DzfxPolicy *pPolicy)
功能	发送地址监控策略
参数	connId，由policy_send_connect调用返回网络连接标识
返回值	true 表示成功，
        false 表示执行失败。
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
	// 监视系统
	case CLZL_DZJS:
		type = htons(PACK_TPYE_DZJS);
		break;
	case CLZL_NRJS:
		type = htons(PACK_TPYE_NRJS);
		break;
	//封堵系统	
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
	//监测
	case CLZL_NRJC:
		type = htons(PACK_TPYE_NRJC);
		break;
	//黑盒
	case CLZL_HHDZJC:
		type = htons(PACK_TPYE_HHDZJC);
		break;
	case CLZL_HHNRJC:
		type = htons(PACK_TPYE_HHNRJC);
		break;
	//过滤字符
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

	body_len = 70;//为了sxwg修改 20090615 / 6 sunguohua
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
原型	bool policy_send_policy(int connId, int cltype, struct DzfxPolicy *pPolicy)
功能	发送内容监控策略
参数	connId，由policy_send_connect调用返回网络连接标识
返回值	true 表示成功，
        false 表示执行失败。
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
	// 监视系统
	case CLZL_DZJS:
		type = htons(PACK_TPYE_DZJS);
		break;
	case CLZL_NRJS:
		type = htons(PACK_TPYE_NRJS);
		break;
	//封堵系统	
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
	//监测
	case CLZL_NRJC:
		type = htons(PACK_TPYE_NRJC);
		break;
	//黑盒
	case CLZL_HHDZJC:
		type = htons(PACK_TPYE_HHDZJC);
		break;
	case CLZL_HHNRJC:
		type = htons(PACK_TPYE_HHNRJC);
		break;
	//过滤字符
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
原型	bool policy_send_policy(int connId, int cltype, struct DzfxPolicy *pPolicy)
功能	发送过滤字符策略
参数	connId，由policy_send_connect调用返回网络连接标识
返回值	true 表示成功，
        false 表示执行失败。
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
	// 监视系统
	case CLZL_DZJS:
		type = htons(PACK_TPYE_DZJS);
		break;
	case CLZL_NRJS:
		type = htons(PACK_TPYE_NRJS);
		break;
	//封堵系统	
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
	//监测
	case CLZL_NRJC:
		type = htons(PACK_TPYE_NRJC);
		break;
	//黑盒
	case CLZL_HHDZJC:
		type = htons(PACK_TPYE_HHDZJC);
		break;
	case CLZL_HHNRJC:
		type = htons(PACK_TPYE_HHNRJC);
		break;
	//过滤字符
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
原型	bool policy_send_data(int connId, int cltype)
功能	策略发送选择
参数	connId，由policy_send_connect调用返回网络连接标识
		cltype， 策略类型
返回值	true 表示成功，
        false 表示执行失败。
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
原型	bool policy_send_finish(int connId)
功能	结束策略发送
参数	connId，由policy_send_connect调用返回网络连接标识
返回值	true，表示执行成功；
        flase，表示执行出错。
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
原型	void policy_send_leave(int index, int result)
功能	将策略发送情况写入策略分发线程执行结果
参数	index，策略发送模块ID，result 任务执行结果
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
原型	void *policy_send(void *p)
功能	策略发送主函数，将以线程形式运行
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
		trapmsg_error_fmt(CENO_SOCKET_CONN_ERR,"策略分发模块与短信监控系统策略接收模块[ip = %s, port = %d]连接监控系统失败,请检查对端的网络状态和控管程序的运行状态!",(*pList)[index]->ip,(*pList)[index]->port);
		policy_send_leave(index, -1);
		return (void *)-1;
	}
	trapmsg_normal_fmt("策略分发模块与短信监控系统策略接收模块[ip = %s, port = %d]连接成功！",(*pList)[index]->ip,(*pList)[index]->port);

	if (!policy_send_notify(sockfd, &info))
	{	
		trapmsg_error_fmt(CENO_SOCKET_SEND_DATA_ERR,"策略分发模块向短信监控系统策略接收模块[ip = %s, port = %d]发送策略分发通知失败,请检查对端的网络状态和控管程序的运行状态!",(*pList)[index]->ip, (*pList)[index]->port);
		close(sockfd);
		policy_send_leave(index, -1);
		return (void *)-1;
	}
	trapmsg_normal_fmt("策略分发模块向短信监控系统策略接收模块[ip = %s, port = %d]发送策略分发通知成功!",(*pList)[index]->ip, (*pList)[index]->port);

	if(!policy_send_recv_ack(sockfd))
	{
		 struct sockaddr_in addr_local;
		 int len_addr = sizeof(struct sockaddr);
		 memset(&addr_local, 0, sizeof(addr_local));
		getsockname(sockfd, (struct sockaddr*)&addr_local, (socklen_t*)&len_addr);
		trapmsg_error_fmt(CENO_SOCKET_RECV_DATA_ERR,"策略分发模块读取短信监控系统策略接收模块[ip = %s, port = %d]策略分发应答失败,请检查对端的网络状态和控管程序的运行状态!",(*pList)[index]->ip, (*pList)[index]->port);
		close(sockfd);
		policy_send_leave(index, -1);
		return (void *)-1;
	}
	trapmsg_normal_fmt("策略分发模块读取短信监控系统策略接收模块[ip = %s, port = %d]策略分发应答成功！",(*pList)[index]->ip, (*pList)[index]->port);

	if(!policy_send_data(sockfd, g_policy_type))
	{
		trapmsg_error_fmt(CENO_SOCKET_SEND_DATA_ERR,"策略分发模块向短信监控系统策略接收模块[ip = %s, port = %d]发送策略失败,请检查对端的网络状态和控管程序的运行状态！",(*pList)[index]->ip, (*pList)[index]->port);
		close(sockfd);
		policy_send_leave(index, -1);
		return (void *)-1;
	}
	trapmsg_normal_fmt("策略分发模块向短信监控系统策略接收模块[ip = %s, port = %d]发送策略成功", (*pList)[index]->ip, (*pList)[index]->port);

	if(!policy_send_finish(sockfd))
	{
		trapmsg_error_fmt(CENO_SOCKET_SEND_DATA_ERR,"策略分发模块向短信监控系统策略接收模块[ip = %s, port = %d]发送策略分发结束通知(接收策略分发结束应答)失败,请检查对端的网络状态和控管程序的运行状态！",(*pList)[index]->ip, (*pList)[index]->port);
		close(sockfd);
		policy_send_leave(index, -1);
		return (void *)-1;
	}
	trapmsg_normal_fmt("策略分发模块向短信监控系统策略接收模块[ip = %s, port = %d]发送策略分发结束通知(接收策略分发结束应答)成功！",(*pList)[index]->ip, (*pList)[index]->port);

	trapmsg_normal_fmt("策略分发模块向[ip = %s, port = %d]发送策略[type = %d]数目[num= %d]成功完成！", (*pList)[index]->ip, (*pList)[index]->port, info.type, info.count);

	close(sockfd);
	policy_send_leave(index, 1);
	return (void *)0;
}
