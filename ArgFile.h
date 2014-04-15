// this is the header file of class ArgFile
// this class manage arguments like windows regtable
// 没有线程案全的保证

#ifndef _ARGFILE_H_
#define _ARGFILE_H_

#include <stdio.h>

#define MAX_FILENAME_LEN     255                         // 文件名最大长度(包括路径)
#define MAX_ARGNAME_LEN      127                         // 参数名或者参数节点名的最大长度
#define MAX_ARGVALUE_LEN     255                         // 参数(文本表示)最大长度
#define MAX_SECTIONPATH_LEN  1023                        // 参数路径的最大长度(不包括参数名)

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
	ARG_SECTION*     m_root;                                  // 参数树根
	int              m_mode;                                  // 打开模式，0只读,1可修改添加
// services
public:
	const char* GetFileName(){return m_fileName;};
	int         GetMode(){return m_mode;};
public:
	int      Create();
	int      Open(const char* filename, int mode = 0);    // 0 只读，1读写.如果遇到读文件出错或者格式错误，返回1，但可能有一部分参数正确读出
	int      Save();
	int      Close();

	struct ARG_SECTION*   GetSection(const char* section_path, struct ARG_SECTION* orgpoint = NULL);
	struct ARG*  GetArg(struct ARG_SECTION* section,const char* argname = NULL);    // 返回的指针是临时的
	struct ARG*  GetArg(const char* section_path,const char* argname = NULL);    // 返回的指针是临时的

	struct ARG_SECTION* AddSection(const char* section_path, struct ARG_SECTION *root);
	struct ARG*         AddArg(struct ARG_SECTION* section, const char* argname, const char* argvalue);
	int      SetArg(struct ARG_SECTION* section, const char* argname, const char* value);

	int      Destroy();

// internal methods
protected:
	void   FreeTree(struct ARG_SECTION *root);

};

#endif
