#ifndef __HEADER_H__
#define __HEADER_H__

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <pthread.h>
#include <syslog.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <fcntl.h>
#include <linux/unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#include <sys/socket.h>
#include <netdb.h>

#include <vector>
#include <list>
using namespace std;

#include "ArgFile.h"
#include "astrapmsg.h" 
#include "Global_Errno.h"

#define  READ_TIMEOUT                    0
#define  WRITE_TIMEOUT                   1

#define  CLZL_DZJS						  1    // 	地址监视 				 
#define  CLZL_NRJS						  2    // 	内容监视 				 
#define  CLZL_DZFX                        3    // 	地址放行 				 
#define  CLZL_DZFD                        4    // 	地址封堵				 
#define  CLZL_NRFD                        5    // 	内容封堵
#define  CLZL_NRJC                        6    //	内容监测
#define  CLZL_HHDZJC                      7    //	黑盒地址监测
#define  CLZL_HHNRJC                      8    //	黑盒内容监测
#define	 CLZL_GLZF						  21	//	代码表过滤字符
#define  CLZL_DZBH                        9		// 	地址保护 

#define  PACK_TPYE_CLFF_START             8101  // 策略分发通知 				 
#define  PACK_TPYE_CLFF_START_ANSWER      8102  // 策略分发通知应答

#define  PACK_TPYE_DZJS                   8201  // 地址监视策略消息				 
#define  PACK_TPYE_NRJS                   8202  // 内容监视策略消息
#define  PACK_TPYE_DZFX		              8203  // 地址放行策略消息 				 
#define  PACK_TPYE_DZFD                   8204  // 地址封堵策略消息 				 
#define  PACK_TPYE_NRFD                   8205  // 内容封堵策略消息 				 
#define  PACK_TPYE_NRJC                   8206  // 内容监测策略消息
#define  PACK_TPYE_HHDZJC                 8207  // 黑盒地址监测策略消息
#define  PACK_TPYE_HHNRJC                 8208  // 黑盒内容监测策略消息
#define  PACK_TPYE_GLZF	                  8211  // 过滤字符策略消息
#define  PACK_TPYE_DZBH		              8209  // 地址保护策略消息

#define  PACK_TPYE_CLFF_END		          8111  // 策略分发完成通知 				 
#define  PACK_TPYE_CLFF_END_ANSWER        8112  // 策略分发完成通知应答

//数据库连接信息
struct DbInfo
{
	char    tnsname[50];
	char    user[50];
	char    pass[50];
};

//目标任务
struct TargetInfo
{
	char    ip[16];
	unsigned short port;
};


//策略任务
struct TaskInfo
{
	int		zxid;	//  中心ID
	int		rwid;	//  任务ID
	int		pzlx;	//  配置类型
	int		zdsj;	//  制定时间
};

//表示一个策略下发的通知信息，包括策略种类、版本信息、条数等信息
struct PolicyInfo
{
	int type;          // 策略种类
	int count;         // 策略条数
	char version[100]; // 策略版本
};

//地址放行策略
struct DzfxPolicy
{
	int		zbid; 		//	总表ID
	int		zxid;		//	中心ID
	char	dz1[30];	//	地址1
	char	dz2[30];	//	地址2
	short	dzms;		//	地址描述
};

//地址封堵策略
struct DzfdPolicy
{
	int		zbid;		//	总表ID
	int		zxid;  		//	中心ID
	char	dz1[30];	//	地址1
	char	dz2[30];	//	地址2
	char	nrpz[256]; 	//	配置内容
	short	dzms;		//	地址描述
};


//地址监控策略
struct DzjkPolicy
{
	int		pzid;		//	配置ID
	int		zxid;  		//	中心ID
	char	dz1[30];	//	地址1
	char	dz2[30];	//	地址2
	short	dzms;		//	地址描述
};

//内容监控策略
struct NrjkPolicy
{
	int		zbid; 		//	中表ID
	int		zxid;		//	中心ID
	char	pznr[256]; 	//	配置内容
};

//过滤字符策略
struct GlzfPolicy
{
	int		id; 		//	总表ID
	char	zfz[5];		//	字符值
	char	bz[4];		//	备注
};

#endif // __HEADER_H__
