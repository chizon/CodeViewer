// this is the header file of class ArgFile
// this class manage arguments like windows regtable
// û���̰߳�ȫ�ı�֤

#ifndef _ARGFILE_H_
#define _ARGFILE_H_

#include <stdio.h>

#define MAX_FILENAME_LEN     255                         // �ļ�����󳤶�(����·��)
#define MAX_ARGNAME_LEN      127                         // ���������߲����ڵ�������󳤶�
#define MAX_ARGVALUE_LEN     255                         // ����(�ı���ʾ)��󳤶�
#define MAX_SECTIONPATH_LEN  1023                        // ����·������󳤶�(������������)

struct ARG
{
	char          argname[MAX_ARGNAME_LEN+1];
	char          argvalue[MAX_ARGVALUE_LEN+1];
	struct ARG*   next;
};
struct ARG_SECTION
{
	char                      node_name[MAX_ARGNAME_LEN+1];
	ARG*                      arglist;
	unsigned short            argcount;

	struct ARG_SECTION*       brother;
	struct ARG_SECTION*       child;
};

class ArgFile
{
// attrs
protected:
	char             m_fileName[MAX_FILENAME_LEN+1];
	ARG_SECTION*     m_root;                                  // ��������
	int              m_mode;                                  // ��ģʽ��0ֻ��,1���޸����
// services
public:
	const char* GetFileName(){return m_fileName;};
	int         GetMode(){return m_mode;};
public:
	int      Create();
	int      Open(const char* filename, int mode = 0);    // 0 ֻ����1��д.����������ļ�������߸�ʽ���󣬷���1����������һ���ֲ�����ȷ����
	int      Save();
	int      Close();

	struct ARG_SECTION*   GetSection(const char* section_path, struct ARG_SECTION* orgpoint = NULL);
	struct ARG*  GetArg(struct ARG_SECTION* section,const char* argname = NULL);    // ���ص�ָ������ʱ��
	struct ARG*  GetArg(const char* section_path,const char* argname = NULL);    // ���ص�ָ������ʱ��

	struct ARG_SECTION* AddSection(const char* section_path, struct ARG_SECTION *root);
	struct ARG*         AddArg(struct ARG_SECTION* section, const char* argname, const char* argvalue);
	int      SetArg(struct ARG_SECTION* section, const char* argname, const char* value);

	int      Destroy();

// internal methods
protected:
	void   FreeTree(struct ARG_SECTION *root);

};

#endif
