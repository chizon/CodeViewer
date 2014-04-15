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

#define  CLZL_DZJS						  1    // 	��ַ���� 				 
#define  CLZL_NRJS						  2    // 	���ݼ��� 				 
#define  CLZL_DZFX                        3    // 	��ַ���� 				 
#define  CLZL_DZFD                        4    // 	��ַ���				 
#define  CLZL_NRFD                        5    // 	���ݷ��
#define  CLZL_NRJC                        6    //	���ݼ��
#define  CLZL_HHDZJC                      7    //	�ںе�ַ���
#define  CLZL_HHNRJC                      8    //	�ں����ݼ��
#define	 CLZL_GLZF						  21	//	���������ַ�
#define  CLZL_DZBH                        9		// 	��ַ���� 

#define  PACK_TPYE_CLFF_START             8101  // ���Էַ�֪ͨ 				 
#define  PACK_TPYE_CLFF_START_ANSWER      8102  // ���Էַ�֪ͨӦ��

#define  PACK_TPYE_DZJS                   8201  // ��ַ���Ӳ�����Ϣ				 
#define  PACK_TPYE_NRJS                   8202  // ���ݼ��Ӳ�����Ϣ
#define  PACK_TPYE_DZFX		              8203  // ��ַ���в�����Ϣ 				 
#define  PACK_TPYE_DZFD                   8204  // ��ַ��²�����Ϣ 				 
#define  PACK_TPYE_NRFD                   8205  // ���ݷ�²�����Ϣ 				 
#define  PACK_TPYE_NRJC                   8206  // ���ݼ�������Ϣ
#define  PACK_TPYE_HHDZJC                 8207  // �ںе�ַ��������Ϣ
#define  PACK_TPYE_HHNRJC                 8208  // �ں����ݼ�������Ϣ
#define  PACK_TPYE_GLZF	                  8211  // �����ַ�������Ϣ
#define  PACK_TPYE_DZBH		              8209  // ��ַ����������Ϣ

#define  PACK_TPYE_CLFF_END		          8111  // ���Էַ����֪ͨ 				 
#define  PACK_TPYE_CLFF_END_ANSWER        8112  // ���Էַ����֪ͨӦ��

//���ݿ�������Ϣ
struct DbInfo
{
	char    tnsname[50];
	char    user[50];
	char    pass[50];
};

//Ŀ������
struct TargetInfo
{
	char    ip[16];
	unsigned short port;
};


//��������
struct TaskInfo
{
	int		zxid;	//  ����ID
	int		rwid;	//  ����ID
	int		pzlx;	//  ��������
	int		zdsj;	//  �ƶ�ʱ��
};

//��ʾһ�������·���֪ͨ��Ϣ�������������ࡢ�汾��Ϣ����������Ϣ
struct PolicyInfo
{
	int type;          // ��������
	int count;         // ��������
	char version[100]; // ���԰汾
};

//��ַ���в���
struct DzfxPolicy
{
	int		zbid; 		//	�ܱ�ID
	int		zxid;		//	����ID
	char	dz1[30];	//	��ַ1
	char	dz2[30];	//	��ַ2
	short	dzms;		//	��ַ����
};

//��ַ��²���
struct DzfdPolicy
{
	int		zbid;		//	�ܱ�ID
	int		zxid;  		//	����ID
	char	dz1[30];	//	��ַ1
	char	dz2[30];	//	��ַ2
	char	nrpz[256]; 	//	��������
	short	dzms;		//	��ַ����
};


//��ַ��ز���
struct DzjkPolicy
{
	int		pzid;		//	����ID
	int		zxid;  		//	����ID
	char	dz1[30];	//	��ַ1
	char	dz2[30];	//	��ַ2
	short	dzms;		//	��ַ����
};

//���ݼ�ز���
struct NrjkPolicy
{
	int		zbid; 		//	�б�ID
	int		zxid;		//	����ID
	char	pznr[256]; 	//	��������
};

//�����ַ�����
struct GlzfPolicy
{
	int		id; 		//	�ܱ�ID
	char	zfz[5];		//	�ַ�ֵ
	char	bz[4];		//	��ע
};

#endif // __HEADER_H__
