// this is the source file of class ArgFile

#include <string.h>
#include <stdlib.h>
//#include <//syslog.h>
#include "ArgFile.h"
/*******************************/
//#include "astrapmsg.h" 
	//#include "Global_Errno.h"
/*******************************/


int      ArgFile::Create()
{
	memset(m_fileName, 0, MAX_FILENAME_LEN +1);
	m_root = NULL;

	return 0;
}


int      ArgFile::Open(const char* filename, int  mode)
{
	FILE*      pFile = NULL;
	char       section_path[MAX_SECTIONPATH_LEN + 1];
	char       argname[MAX_ARGNAME_LEN+1];
	char       argvalue[MAX_ARGVALUE_LEN+1];
	char      *p1, *p2, *p3;        // section_path, argname, argvalue
	struct ARG_SECTION* section;

	m_mode = mode;

	memset(m_fileName, 0, MAX_FILENAME_LEN+1);
	strcpy(m_fileName, filename);

	pFile= fopen(m_fileName, "r");

	if(pFile)
	{
		p1 = section_path;

		while(1)
		{
			if ( fread(p1, 1, 1, pFile) == 1 )
			{
				// 滤掉空白、换行
				if (*p1 == '\n' || *p1 == '\r' || *p1 == ' ' || *p1 == '\t' )
				{
					continue;
				}
				// 滤掉注释
				else if ( *p1 == '#' )
				{
					while( 1 )
					{
						if( fread( p1, 1, 1, pFile ) == 1 )
						{
							if ( *p1 != '\r' && *p1 != '\n') 
							{
								continue;
							}
							else
							{
								break;
							}
						}
						else
						{
							goto out_open;
						}
					}
				}
				else
				{
					break;
				}
			}
			else
			{
				goto out_open;
			}
		}

		// 节开始
		if(*p1 == '[')
		{
open_1:
			memset(section_path, 0, MAX_SECTIONPATH_LEN + 1);
			p1 = section_path;

			if( fread(p1, 1, 1, pFile) == 1 
				&& ((*p1 >= 'a' && *p1 <= 'z') || (*p1 >= 'A' && *p1 <= 'Z')))
			{
				p1 ++;
			}
			else
			{
				// 读文件错误或文件格式有错
				//syslog(LOG_ALERT,"读文件[%s] 错误或文件格式有错");
				//trapmsg_error(CENO_FILE_READ_ERROR, m_fileName);
				return 1;
			}
open_2:
			while( fread(p1, 1, 1, pFile) == 1 
					&& (*p1 == '_' 
					|| (*p1 >= 'a' && *p1 <= 'z') 
					|| (*p1 >= 'A' && *p1 <= 'Z') 
					|| (*p1 >= '0' && *p1 <= '9') ) )
			{
				p1 ++;
			}

			// 节点
			if(*p1 == '/')
			{
				p1++;
				goto open_2;
			}
			// 节即将结束
			else if(*p1 == ']')
			{
				// 滤掉末尾空白
				while( fread(p1, 1, 1, pFile) == 1 && ( *p1 == '\t' || *p1 == ' '));

				// 滤掉末尾注释
				if( *p1 == '#' )
				{
					while( fread(p1, 1, 1, pFile) == 1 && ( *p1 != '\r' && *p1 != '\n'));
				}

				// 节结束
				if( (*p1 == '\r' || *p1 == '\n'))
				{
					*p1 = 0;   // section_path end
					// 调用内部方法添加section
					section = this->AddSection(section_path, NULL);
					if(section == NULL)
					{
						// 内存分配错误或参数文件格式错误
						//syslog(LOG_ALERT,"内存分配失败，报告错误");
						//trapmsg_error(CENO_MEM_MALLOC_ERROR);
						return 1;
					}
					//
				}
				else
				{
					goto out_open;
				}

				while( 1 )
				{
					if ( fread(p1, 1, 1, pFile) == 1 )
					{
						// 滤掉空白、换行
						if (*p1 == '\r' || *p1 == '\n' || *p1 == '\t' || *p1 == ' ' )
						{
							continue;
						}
						// 滤掉注释
						else if ( *p1 == '#' )
						{
							while( fread(p1, 1, 1, pFile) == 1 && ( *p1 != '\r' && *p1 != '\n'));
						}
						else
						{
							break;
						}
					}
					else
					{
						goto out_open;
					}
				}

				if(*p1 == '[')
				{
					goto open_1;
				}
				// 参数名开始
				else if((*p1 >= 'a' && *p1 <= 'z') || (*p1 >= 'A' && *p1 <= 'Z'))
				{
					memset(argname, 0, MAX_ARGNAME_LEN + 1);
					p2 = argname;
					*p2 = *p1;
open_3:
					p2++;

					while( fread(p2, 1, 1, pFile) == 1 
							&& (*p2 == '_' 
							|| (*p2 >= 'a' && *p2 <= 'z') 
							|| (*p2 >= 'A' && *p2 <= 'Z') 
							|| (*p2 >= '0' && *p2 <= '9') ) )
					{
						p2++;
					}
					// 参数名将结束，滤掉空白
					while( *p2 == ' ' || *p2 == '\t')
					{
						if(fread(p2, 1, 1, pFile) != 1) break;
					}
					// 参数名结束
					if(*p2 == '=')
					{
						*p2 = 0;
						p3 = argvalue;
						memset(argvalue, 0, MAX_ARGVALUE_LEN + 1);

						// 参数值将开始，滤掉空白
						while( fread(p3, 1, 1, pFile) == 1 && (*p3 == ' ' || *p3 == '\t'));
						// 参数值开始
						if( *p3 == '{')
						{
							while( fread(p3, 1, 1, pFile) == 1 && *p3 != '}') p3++;
							// 参数值结束
							if(*p3 == '}')
							{
								// 在 section下增加一个 argname(value)
								*p3 = 0;
								if(NULL == AddArg(section, argname, argvalue))
								{
									// 内存错误或参数格式错误
									//syslog(LOG_ALERT,"内存分配失败，报告错误");
									//trapmsg_error(CENO_MEM_MALLOC_ERROR);
									return 1;
								}
								p3 = argvalue;
							}
							else
							{
								return 1;             // 读文件错误或文件格式有错
							}

							// 滤掉参数值后的空白
							while( fread(p3, 1, 1, pFile) == 1 && (*p3 == ' ' || *p3 == '\t'));
							// 滤掉参数值后的注释
							if ( *p3 == '#' )
							{
								while( fread(p3, 1, 1, pFile) == 1 && (*p3 != '\n'  && *p3 != '\r'));
							}

							if(*p3 == '\n'  || *p3 == '\r')
							{
								while( 1 )
								{
									if ( fread(p3, 1, 1, pFile) == 1 )
									{
										// 滤掉空白、换行
										if (*p3 == '\r' || *p3 == '\n' || *p3 == '\t' || *p3 == ' ' )
										{
											continue;
										}
										// 滤掉注释
										else if ( *p3 == '#' )
										{
											while( fread(p3, 1, 1, pFile) == 1 && ( *p3 != '\r' && *p3 != '\n'));
										}
										else
										{
											break;
										}
									}
									else
									{
										goto out_open;
									}
								}

								// 下一个节
								if( *p3 == '[')
								{
									p1 = section_path;
									*p1 = *p3;
									goto open_1;
								}
								// 下一个参数
								else if((*p3 >= 'a' && *p3 <= 'z') || (*p3 >= 'A' && *p3 <= 'Z'))
								{
									memset(argname, 0, MAX_ARGNAME_LEN + 1);
									p2 = argname;
									*p2 = *p3;
									goto open_3;
								}
								else
								{
									goto out_open;
								}
							}
							else
							{
								goto out_open;
							}
						}
						else
						{
							goto out_open;
						}
					}
					else
					{
						goto out_open;
					}
				}
				else
				{
					goto out_open;
				}
			}
			else
			{
				goto out_open;
			}
		}
		else
		{
			goto out_open;
		}
	}
	else
	{
		goto out_open;
	}

out_open:
	if( pFile == NULL )
	{
		// file error
		return -1;             // 打开文件失败
	}
	else if(feof(pFile))
	{
		fclose(pFile);
		return 0;
	}
	else if( ferror(pFile) )
	{
		// file error
		fclose(pFile);
		return -1;             // 读文件错误
	}
	else
	{
		// format error
		fclose(pFile);
		return 1;              // 文件格式有错
	}
}

int      ArgFile::Close()
{
	FreeTree(m_root);
	m_root = NULL;
	return 0;
}

int      ArgFile::Destroy()
{
	Close();
	return 0;
}

struct ARG_SECTION*   ArgFile::GetSection(const char* section_path, ARG_SECTION* orgpoint)
{
	char                  part_section_name[MAX_ARGNAME_LEN+1];
	char*                 p       = (char*)section_path;
	char*                 temp    = NULL;
	struct ARG_SECTION*   section = orgpoint;

	while(1)
	{
		if(temp == NULL)
		{
			temp = part_section_name;
			memset(temp, 0, MAX_ARGNAME_LEN+1);
		}

		if(temp - part_section_name > MAX_ARGNAME_LEN) 
		{
			// 参数错误，太长
			break;
		}

		if(*p == '/' || *p == 0)
		{
			temp = NULL;

			if(section == NULL)
			{
				section = m_root;
			}
			else
			{
				section = section->child;
			}

			if(section == NULL)
			{
				break;
			}
			else
			{
				while(section && strcmp(section->node_name, part_section_name) != 0)
				{
					section = section->brother;
				}

				if(section == NULL)
				{
					break;
				}
			}

			if(*p == 0)
			{
				break;
			}

		}
		else
		{
			*temp = *p;
			temp++;
		}

		p++; 
	}

	return section;
}

struct ARG*  ArgFile::GetArg(ARG_SECTION* section,const char* argname)
{
	if(argname == NULL)
	{
		return section->arglist;
	}
	else
	{
		struct ARG* arg = section->arglist;
		while(arg)
		{
			if( strcmp(arg->argname, argname) == 0 )
			{
				return arg;
			}
			else
			{
				arg = arg->next;
			}
		}
		return NULL;
	}
}

struct ARG*  ArgFile::GetArg(const char* section_path,const char* argname)
{
	ARG_SECTION* n_section = GetSection(section_path);
	if(n_section)
		return GetArg(n_section, argname);
	else
		return NULL;
}



struct ARG_SECTION* ArgFile::AddSection(const char* section_path, struct ARG_SECTION *root)
{
	struct ARG_SECTION *parent, *brother, *section;
	char  part_section_path[MAX_ARGNAME_LEN + 1];
	char* p = (char*)section_path;
	char* p_part_section;

	if(section_path == NULL || *section_path == 0)
	{
		return NULL;
	}

	memset(part_section_path, 0, MAX_ARGNAME_LEN+1);
	p_part_section = part_section_path;
	parent = root;
	brother = NULL;
	if(root == NULL)
	{
		section = m_root;
	}
	else
	{
		section = root->child;
	}

	while(1)
	{
		if(*p == '/' || *p == 0)
		{
			while(section)
			{
				if(strcmp(section->node_name, part_section_path) == 0)
				{
					break;
				}
				else
				{
					brother = section;
					section = section->brother;
				}
			}
			if(section == NULL)
			{
				section = (struct ARG_SECTION*)malloc(sizeof(struct ARG_SECTION));

				strcpy(section->node_name, part_section_path);
				section->argcount = 0;
				section->arglist = NULL;
				section->brother = NULL;
				section->child = NULL;

				if(brother)
				{
					brother->brother = section;
				}
				else if(parent)
				{
					parent->child = section;
				}
				else
				{
					m_root = section;
				}
			}

			if(*p == 0)
			{
				return section;
			}

			parent = section;
			section = section->child;
			brother = NULL;
			memset(part_section_path, 0, MAX_ARGNAME_LEN+1);
			p_part_section = part_section_path;
		}
		else
		{
			*p_part_section = *p;
			p_part_section ++;
		}
		p ++;
	}
}

struct ARG*   ArgFile::AddArg(struct ARG_SECTION* section, const char* argname, const char* argvalue)
{
	if(section == NULL)
	{
		return NULL;
	}

	struct ARG* arg, *before = NULL;
	arg = section->arglist;
	while(arg)
	{
		if(strcmp(arg->argname, argname) == 0)
		{
			strcpy(arg->argvalue, argvalue);
			return arg;
		}
		before = arg; arg = arg->next;
	}

	arg = (struct ARG*)malloc(sizeof(struct ARG));
	strcpy(arg->argname, argname);
	strcpy(arg->argvalue, argvalue);
	arg->next = NULL;

	if(before)before->next = arg;
	else
		section->arglist = arg;

	return arg;
}

//删除树
void ArgFile::FreeTree(struct ARG_SECTION *root)
{
	ARG*  arg;
	if(root != NULL)
	{
		FreeTree(root->child);
        FreeTree(root->brother);
		arg = root->arglist;
		while(arg)
		{
			ARG* p = arg;
			arg = arg->next;
			free(p);
		}
		free(root);
    }
} 
////////////

///////////////
// test
/*
int main()
{
	ArgFile   file;
	file.Create();
	if(0 != file.Open("SmFilterServer.cfg"))
	{
		printf("failed to open arg file\n");
		return 0;
	}

	// 系统初始化
	struct ARG*         n_pArg = NULL;
	struct ARG_SECTION* n_pSection = NULL, *ptemp = NULL;
	struct ARG_SECTION* section[256];
	int         count = 0;

	

	n_pSection = file.GetSection("SERVER", NULL);
	while( n_pSection)
	{
		n_pArg = file.GetArg(n_pSection, NULL);
		while(n_pArg)
		{
			printf("%s = %s\n", n_pArg->argname, n_pArg->argvalue);
			n_pArg = n_pArg->next;
		}
back:
		if(n_pSection->child)
		{
			section[count++] = n_pSection;
			n_pSection = n_pSection->child;
		}
		else
		{
			n_pSection = n_pSection->brother;
		}

		if(!n_pSection && count > 0)
		{
			while(!n_pSection && count > 0)
			{
				n_pSection = section[--count];
				n_pSection = n_pSection->brother;
			}

			if( n_pSection) goto back;
		}
	}

	return 0;
}
*/
//
