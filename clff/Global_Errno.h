//ԭ�е� errno
#define CENO_SYS_CALL_ERROR                 "001001"		/*ϵͳ����ʧ�� API����:%s                			*/
#define CENO_SYS_CALL_ERROR_STR             "001002"		/*ϵͳ����ʧ�� API����:%s ERR:%s         			*/
#define CENO_DB_CONN_ERROR_STR              "002001"		/*���ݿ�����ʧ�� ENO:%d %s               			*/
#define CENO_DB_EXCU_ERROR_STR              "002002"		/*���ݿ�SQLִ��ʧ�� ENO:%d %s            			*/
#define CENO_DB_DISCONN_ERROR_STR           "002003"		/*���ݿ�Ͽ�ʧ�� ENO:%d %s               			*/
#define CENO_EXPORT_DBTAB_ERROR_STR         "002011"		/*�������ݿ��ʧ�� ENO:%d %s             			*/
#define CENO_INPORT_DBTAB_ERROR_STR         "002012"		/*�������ݿ��ʧ�� ENO:%d %s             			*/
#define CENO_UPDATE_VIEW_ERROR_STR          "002021"		/*�޷����±�/��ͼ ENO:%d %s              			*/
#define CENO_FILE_READ_ERROR                "003001"		/*�ļ�����ʧ�� �ļ���:%s                 			*/
#define CENO_FILE_FORMATE_ERROR             "003002"		/*�ļ���ʽ���� �ļ���:%s                 			*/
#define CENO_FILE_WRITE_ERROR               "003003"		/*�ļ�д��ʧ�� �ļ���:%s				 			*/
#define CENO_FILE_CREATE_ERROR              "003004"		/*�ļ�����ʧ�� �ļ���:%s                 			*/
#define CENO_FILE_DELETE_ERROR              "003005"		/*�ļ�ɾ��ʧ�� �ļ���:%s                 			*/
#define CENO_FILE_OPEN_ERROR                "003006"		/*�ļ���ʧ�� �ļ���:%s                 			*/
#define CENO_FILE_EXCU_ERROR                "003007"		/*�ļ�ִ��ʧ�� �ļ���:%s                 			*/
#define CENO_FILE_RENAME_ERROR              "003008"		/*�ļ�������ʧ�� �ļ���:%s               			*/
#define CENO_FILE_LOCATE_ERROR              "003009"		/*�ļ���λ���� �ļ���:%s                 			*/
#define CENO_FILE_TIME_ERROR                "003010"		/*�ļ�ʱ����� �ļ���:%s                 			*/
#define CENO_FILE_LEN_ERROR                 "003011"		/*�ļ����ȴ��� �ļ���:%s                 			*/
#define CENO_FILE_CTL_ERROR                 "003012"		/*�ļ�CTL���� �ļ���:%s                  			*/
#define CENO_FILE_LOCK_ERROR                "003013"		/*�ļ�LOCK���� �ļ���:%s                 			*/
#define CENO_FILE_MAP_ERROR                 "003014"		/*�ļ�MAP���� �ļ���:%s                  			*/
#define CENO_DIR_CREATE_ERROR               "003101"		/*Ŀ¼����ʧ�� Ŀ¼��:%s                 			*/
#define CENO_DIR_DELETE_ERROR               "003102"		/*Ŀ¼ɾ��ʧ�� Ŀ¼��:%s                 			*/
#define CENO_MEM_MALLOC_ERROR               "001901"		/*�ڴ�������                           			*/
#define CENO_MEM_MALLOC_ERROR_LOCATE        "001902"		/*�ڴ������� File:%s Line:%d           			*/
#define CENO_EXCU_PARAM_ERROR_STR           "001921"		/*ִ�в�������ID:%d��Desc:%s           			*/
#define CENO_PTHREAD_CREATE_ERROR_STR       "001941"		/*�̴߳���ʧ�ܣ�Desc:%s                  			*/
#define CENO_PROCESS_CREATE_ERROR_STR       "001942"		/*���̴���ʧ�ܣ�Desc:%s                  			*/
#define CENO_SHM_CREATE_ERROR_STR           "004000"		/*SHM��������ID:%d��Desc:%s            			*/
#define CENO_SHM_ALEXIST_ERROR_STR          "004001"		/*�����ڴ��Ѿ����ڣ�ID:%d��Desc:%s       			*/
#define CENO_SHM_LINK_ERROR_STR             "004002"		/*SHM���Ӵ���ID:%d��Desc:%s            			*/
#define CENO_SHM_MAT_ERROR_STR              "004003"		/*SHM MAT����ID:%d��Desc:%s            			*/
#define CENO_SHM_CTL_ERROR_STR              "004004"		/*SHM CTL����ID:%d��Desc:%s            			*/
#define CENO_SHM_FREE_ERROR_STR             "004005"		/*SHM�ͷŴ���ID:%d��Desc:%s            			*/
#define CENO_MESG_QUE_CREATE_ERROR_STR      "004006"		/*��Ϣ���д�������ID:%d��Desc:%s       			*/
#define CENO_MESG_QUE_ALEXIST_ERROR_STR     "004007"		/*��Ϣ�����Ѿ����ڣ�ID:%d��Desc:%s       			*/
#define CENO_MESG_QUE_LINK_ERROR_STR        "004008"		/*��Ϣ�������Ӵ���ID:%d��Desc:%s       			*/
#define	CENO_MESG_QUE_FREE_ERROR_STR        "004009"		/*��Ϣ�����ͷŴ���ID:%d��Desc:%s       			*/
#define	CENO_MESG_QUE_CTL_ERROR_STR         "004010"		/*��Ϣ����CTL����ID:%d��Desc:%s        			*/
#define CENO_MESG_QUE_READ_ERROR_STR        "004011"		/*��Ϣ���ж�����ID:%d��Desc:%s         			*/
#define CENO_MESG_QUE_WRITE_ERR_STR         "004012"		/*��Ϣ����д����ID:%d��Desc:%s					*/
#define CENO_SIG_CRE_ERR                    "004013"		/*�ź�����������ID:%d��Desc:%s					*/
#define CENO_SIG_EXIST                      "004014"		/*�ź����Ѿ����ڣ�ID:%d��Desc:%s					*/
#define CENO_SIG_CONN_ERR                   "004015"		/*�ź������Ӵ���ID:%d��Desc:%s					*/
#define CENO_SIG_CTL_ERR                    "004016"		/*�ź���CTL����ID:%d��Desc:%s						*/
#define CENO_SIG_OP_ERR                     "004017"		/*�ź���OP����ID:%d��Desc:%s						*/
#define CENO_RELE_SIG_ERR                   "004018"		/*�ź����ͷŴ���ID:%d��Desc:%s					*/
#define CENO_CRE_PIPE_ERR                   "004019"		/*�ܵ�������������:%s								*/
#define CENO_OPEN_PIPE_ERR                  "004020"		/*�ܵ��򿪴�������:%s								*/
#define CENO_OCI_TRANSFER_ERR               "002101"		/*OCI����ʧ�ܣ�API����:%s ��;:%s					*/
#define CENO_SEND_DATA_ERR_I                "005001"		/*��������ʧ��,Remote_IP:%s Remote_Port:%d          */
#define CENO_ACCEPT_DATA_ERR_I              "005002"		/*��������ʧ�ܣ�Local_Port:%d						*/
#define CENO_ACCEPT_DATA_ERR_RILP           "005003"		/*��������ʧ�ܣ�Remote_IP:%s Local_Port:%d  		*/
#define CENO_CONN_ERR                       "005004"		/*����ʧ��,Remote_IP:%s Remote_Port:%d   			*/
#define CENO_DISCONN                        "005005"		/*�����쳣�Ͽ�,Remote_IP:%s Remote_Port:%d       	*/
#define CENO_SEND_DATA_ERR_II               "005050"		/*��������ʧ��,Remote_IP:%s Remote_Port:%d          */
#define CENO_ACCEPT_DATA_ERR_LILP           "005051"		/*��������ʧ��, Local_IP:%s Local_Port:%d			*/
#define CENO_ACCEPT_DATA_ERR_II             "005052"		/*��������ʧ��, Local_Port:%d            			*/
#define CENO_SOCKET_CREAT_ERR               "005101"		/*SOCKET��������                         			*/
#define CENO_SOCKET_BIND_ERR                "005102"		/*SOCKET�˿ڰ󶨴���IP:%s Port:%d      			*/
#define CENO_SOCKET_CONN_ERR                "005103"		/*SOCKET���Ӵ���IP:%s Port:%u          			*/
#define CENO_SOCKET_SEND_DATA_ERR           "005104"		/*SOCKET �������ݴ���Remote_IP:%s Remote_Port:%d  */
#define CENO_SOCKET_RECV_DATA_ERR           "005105"		/*SOCKET �������ݴ���Local_IP:%s Local_Port:%d    */
#define CENO_SOCKET_CLOSE_ERR               "005106"		/*SOCKET �رմ���                        			*/
#define CENO_SOCKET_CONN_INVALIDATION       "005107"		/*SOCKET ����ʧЧ��Remote_IP:%s Remote_Port:%d      */
#define CENO_SOCKET_DISCONN                 "005108"		/*SOCKET ���ӶϿ���Remote_IP:%s Remote_Port:%d      */
#define CENO_SOCKET_ACCEPT_ERR              "005109"		/*SOCKET ���ӽ���ʧ�ܣ�Remote_IP:%s Remote_Port:%d  */
#define CENO_SOCKET_LISTEN_ERR              "005110"		/*SOCKET ����ʧ�ܣ�IP:%s Port:%d         			*/
#define CENO_SOCKET_CONN_ACHIVEV_UPPERLIMIT "005111"		/*SOCKET �������ﵽ����                  			*/
#define CENO_CORBA_INITIALIZATION_ABNORMITY "005901"		/*CORBA��ʼ���쳣                        			*/
#define CENO_CORBA_RELEASE_ABNORMITY        "005902"		/*CORBA�ͷ��쳣                          			*/
#define CENO_OPERATION_CHAIN_ESTABLISH_ERR  "006001"		/*ҵ����·����ʧ�ܣ�Remote_IP:%s Remote_Port:%d     */
#define CENO_OPERATION_DATA_SEND_ERR        "006002"		/*ҵ�����ݷ���ʧ�ܣ�Remote_IP:%s Remote_Port:%d     */
#define CENO_OPERATION_DATA_ACCEPT_ERR      "006003"		/*ҵ�����ݽ���ʧ�ܣ�Remote_IP:%s Remote_Port:%d     */
#define CENO_OPERATION_DATA_ERR             "006004"		/*ҵ���������ݴ���                       			*/
#define CENO_OPERATION_CHAIN_OVERTIME       "006005"		/*ҵ����·������ʱ                       			*/
#define CENO_OPERATION_CHAIN_DISCONN        "006006"		/*ҵ����·�쳣�Ͽ��� Desc:%s             			*/

//�����
//errno
#define CENO_INIT_RESOURCE_LIST             "001908"		/*��ʼ����Դ����ʧ��!*/
#define CENO_INIT_BUFFER_LIST               "001909"		/*��ʼ����������ʧ��!*/
#define CENO_INIT_REGISTER_LIST				"001910"		/*��ʼ�����ŵǼ�����ʧ��!*/
#define CENO_LOGIN_SMC                      "006007"		/*����������֤ʧ�� IP:%s PORT:%d*/
#define CENO_RECVER_MAXTHREADS              "006009"		/*�����̴߳ﵽ���ֵ*/ 
#define CENO_DB_LOADDATA_ERR_STR			"002009"		/*%s�������%sʧ��*/
#define CENO_OPERATION_SMTYPE_ERROR_STR		"006010"		/*�޷�ʶ��Ķ������� Des: %s*/
#define CENO_SMPARSER_ERROR_STR				"006011"		/*���ŷ���ʧ��, DES: %d*/
#define CENO_DB_MSG_ERROR_STR			    "002005"		/*���ݿ�1��2����Ϣ��һ�� ENO:%d %s*/
#define CENO_DB_NOFIND_VALID_ZK_STR		    "002010"		/*û���ҵ���Ч������Ϣ ENO:%d %s*/
#define CENO_MAIN_FAILED_INIT_LISTEN        "001911"		/*��ʼ���������ģ��ʧ��*/
#define CENO_MAIN_FAILED_INIT_EXEC          "001912"		/*��ʼ������ִ��ģ��ʧ��*/
#define	CENO_SOCKET_SELECT_TIMEOUT_R        "005112"		/*SOCKET �ȴ��ɶ���ʱ��Errno:%d*/
#define CENO_DB_CONN_ERROR_STR2             "002013"		/*���ݿ�����ʧ�� ENO:%s*/
#define CENO_DB_EXCU_ERROR_STR2             "002014"		/*���ݿ�SQLִ��ʧ�� ENO:%s*/
#define CENO_POLICY_REQUEST_FAIL            "006012"		/*%s��������ʧ��*/
#define CENO_BUFF_FILEMANFAILED             "001903"		/*���̻����ļ�����������ʧ�ܣ����̻��湦��ʧЧ*/
#define CENO_BUFF_FAILEDREAD                "001904"		/*�������ļ�ʧ��*/
#define CENO_ERRDATALEN                     "001905"		/*�Ƿ��Ļ������ݳ���*/
#define CENO_BUFF_FAILEDWRITE               "001906"		/*д�����ļ�ʧ��*/
#define CENO_DATACHAN_FAILEDCREATEBUFF      "001907"		/*����������ʧ��*/
#define CENO_RECVER_FAILEDINITTHREADOBJ     "001944"		/*�̶߳����ʼ��ʧ��*/
#define CENO_RECVER_FAILEDRUNTHREAD         "001945"		/*�̶߳�������ʧ��*/
#define CENO_MAIN_FAILEDINITDATACHAN        "006013"		/*��ʼ�����ݷ���ͨ��ģ��ʧ��*/
#define CENO_FAILED_CREATE_AUTOMAC          "006014"		/*"�Զ�������ʧ�ܣ���Ӧ�����ļ� %s"*/
#define CESTR_FAILED_CREATE_AUTOMAC         "�Զ�������ʧ�ܣ���Ӧ�����ļ� %s"
#define CENO_SEND_DATA_TIMEOUT              "005122"
#define CESTR_SEND_DATA_TIMEOUT             "����%s���ݳ�ʱ IP:%s"
#define CENO_SEND_DATA_NOTCPLINK            "005123"
#define CESTR_SEND_DATA_NOTCPLINK           "����%s����û���ҵ���Ӧ����· IP:%s"


//success
#define CSUC_CREATE_THREAD_SUCCESS			"012002"		/*%s�̴߳����ɹ� Thread:%d Pid:%d!*/
#define CSUC_APP_LINK_SUCESS				"012003"		/*ҵ����·�����ɹ� IP:%s PORT:%d!*/
#define CSUC_DB_CONN_SUCCESS                "012004"		/*���ݿ����ӳɹ� Desc:%s*/
#define CENO_DB_FIND_VALID_ZK_STR			"012005"		/*�ҵ���Ч������Ϣ ENO:%d %s*/
#define CENO_POLICY_REQUEST_SUCCESS         "012006"		/*%s��������ɹ�*/
#define CSUC_OPERATION_AUTOMACHINE_CRT      "016008"		/*�Զ�������ɹ�, ��Ӧ�����ļ�: %s*/
