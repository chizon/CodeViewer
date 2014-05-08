// snmptest.cpp : 定义控制台应用程序的入口点。
//

#include <iostream>
#include <snmp_pp/snmp_pp.h>
#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"snmp_pp.lib")
using namespace std;
#define SYSDESCR "1.3.6.1.2.1.2.2.1.6.2"
int main()
{
	Snmp::socket_startup();
	int status;
	CTarget ctarget((IpAddress)"192.168.2.57");
	Vb vb(SYSDESCR);
	Pdu pdu;

	Snmp snmp(status);
	if(status!=SNMP_CLASS_SUCCESS)
	{
		cout<<snmp.error_msg(status);
		return -1;
	}

	pdu+=vb;
	if((status = snmp.get(pdu,ctarget))!=SNMP_CLASS_SUCCESS)
	{
		cout<<snmp.error_msg(status)<<endl;
		return -1;
	}
	else
	{
		pdu.get_vb(vb,0);
		cout<<"System Descriptor = "<<vb.get_printable_value()<<endl;
		return 0;
	}
	/*status = SNMP_CLASS_SUCCESS;
	while(status==SNMP_CLASS_SUCCESS)
	{
		if((status = snmp.get_next(pdu,ctarget))==SNMP_CLASS_SUCCESS)
		{
			pdu.get_vb(vb,0);
			cout<<"Mib Object = "<<vb.get_printable_oid()<<endl;
			cout<<"Mib Value = "<<vb.get_printable_value()<<endl;
			pdu.set_vb(vb,0);
		}
	}*/
	Snmp::socket_cleanup();
	return 0;
}
