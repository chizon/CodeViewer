#ifndef __RECORDSET_H__
#define __RECORDSET_H__

#include <string>
using namespace std;
#include "Proc_utils.h"

class Database;
class Recordset
{
public:
	Database*    m_pdatabase;
	Proc_Result  m_proc_set;

public:
	Recordset(Database* pdatabase);
	virtual ~Recordset();

	bool   Open(const char* sql);
	bool   Close();
	bool   Next();
	bool   IsEOF();
	string GetStringValue(int index);
	int    GetIntValue(int index);
};

#endif
