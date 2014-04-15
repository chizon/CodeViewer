//原有的 errno
#define CENO_SYS_CALL_ERROR                 "001001"		/*系统调用失败 API名称:%s                			*/
#define CENO_SYS_CALL_ERROR_STR             "001002"		/*系统调用失败 API名称:%s ERR:%s         			*/
#define CENO_DB_CONN_ERROR_STR              "002001"		/*数据库连接失败 ENO:%d %s               			*/
#define CENO_DB_EXCU_ERROR_STR              "002002"		/*数据库SQL执行失败 ENO:%d %s            			*/
#define CENO_DB_DISCONN_ERROR_STR           "002003"		/*数据库断开失败 ENO:%d %s               			*/
#define CENO_EXPORT_DBTAB_ERROR_STR         "002011"		/*导出数据库表失败 ENO:%d %s             			*/
#define CENO_INPORT_DBTAB_ERROR_STR         "002012"		/*导入数据库表失败 ENO:%d %s             			*/
#define CENO_UPDATE_VIEW_ERROR_STR          "002021"		/*无法更新表/视图 ENO:%d %s              			*/
#define CENO_FILE_READ_ERROR                "003001"		/*文件读入失败 文件名:%s                 			*/
#define CENO_FILE_FORMATE_ERROR             "003002"		/*文件格式错误 文件名:%s                 			*/
#define CENO_FILE_WRITE_ERROR               "003003"		/*文件写入失败 文件名:%s				 			*/
#define CENO_FILE_CREATE_ERROR              "003004"		/*文件创建失败 文件名:%s                 			*/
#define CENO_FILE_DELETE_ERROR              "003005"		/*文件删除失败 文件名:%s                 			*/
#define CENO_FILE_OPEN_ERROR                "003006"		/*文件打开失败 文件名:%s                 			*/
#define CENO_FILE_EXCU_ERROR                "003007"		/*文件执行失败 文件名:%s                 			*/
#define CENO_FILE_RENAME_ERROR              "003008"		/*文件重命名失败 文件名:%s               			*/
#define CENO_FILE_LOCATE_ERROR              "003009"		/*文件定位错误 文件名:%s                 			*/
#define CENO_FILE_TIME_ERROR                "003010"		/*文件时间错误 文件名:%s                 			*/
#define CENO_FILE_LEN_ERROR                 "003011"		/*文件长度错误 文件名:%s                 			*/
#define CENO_FILE_CTL_ERROR                 "003012"		/*文件CTL错误 文件名:%s                  			*/
#define CENO_FILE_LOCK_ERROR                "003013"		/*文件LOCK错误 文件名:%s                 			*/
#define CENO_FILE_MAP_ERROR                 "003014"		/*文件MAP错误 文件名:%s                  			*/
#define CENO_DIR_CREATE_ERROR               "003101"		/*目录创建失败 目录名:%s                 			*/
#define CENO_DIR_DELETE_ERROR               "003102"		/*目录删除失败 目录名:%s                 			*/
#define CENO_MEM_MALLOC_ERROR               "001901"		/*内存分配错误                           			*/
#define CENO_MEM_MALLOC_ERROR_LOCATE        "001902"		/*内存分配错误 File:%s Line:%d           			*/
#define CENO_EXCU_PARAM_ERROR_STR           "001921"		/*执行参数错误，ID:%d，Desc:%s           			*/
#define CENO_PTHREAD_CREATE_ERROR_STR       "001941"		/*线程创建失败，Desc:%s                  			*/
#define CENO_PROCESS_CREATE_ERROR_STR       "001942"		/*进程创建失败，Desc:%s                  			*/
#define CENO_SHM_CREATE_ERROR_STR           "004000"		/*SHM创建错误，ID:%d，Desc:%s            			*/
#define CENO_SHM_ALEXIST_ERROR_STR          "004001"		/*共享内存已经存在，ID:%d，Desc:%s       			*/
#define CENO_SHM_LINK_ERROR_STR             "004002"		/*SHM连接错误，ID:%d，Desc:%s            			*/
#define CENO_SHM_MAT_ERROR_STR              "004003"		/*SHM MAT错误，ID:%d，Desc:%s            			*/
#define CENO_SHM_CTL_ERROR_STR              "004004"		/*SHM CTL错误，ID:%d，Desc:%s            			*/
#define CENO_SHM_FREE_ERROR_STR             "004005"		/*SHM释放错误，ID:%d，Desc:%s            			*/
#define CENO_MESG_QUE_CREATE_ERROR_STR      "004006"		/*消息队列创建错误，ID:%d，Desc:%s       			*/
#define CENO_MESG_QUE_ALEXIST_ERROR_STR     "004007"		/*消息队列已经存在，ID:%d，Desc:%s       			*/
#define CENO_MESG_QUE_LINK_ERROR_STR        "004008"		/*消息队列连接错误，ID:%d，Desc:%s       			*/
#define	CENO_MESG_QUE_FREE_ERROR_STR        "004009"		/*消息队列释放错误，ID:%d，Desc:%s       			*/
#define	CENO_MESG_QUE_CTL_ERROR_STR         "004010"		/*消息队列CTL错误，ID:%d，Desc:%s        			*/
#define CENO_MESG_QUE_READ_ERROR_STR        "004011"		/*消息队列读错误，ID:%d，Desc:%s         			*/
#define CENO_MESG_QUE_WRITE_ERR_STR         "004012"		/*消息队列写错误，ID:%d，Desc:%s					*/
#define CENO_SIG_CRE_ERR                    "004013"		/*信号量创建错误，ID:%d，Desc:%s					*/
#define CENO_SIG_EXIST                      "004014"		/*信号量已经存在，ID:%d，Desc:%s					*/
#define CENO_SIG_CONN_ERR                   "004015"		/*信号量连接错误，ID:%d，Desc:%s					*/
#define CENO_SIG_CTL_ERR                    "004016"		/*信号量CTL错误，ID:%d，Desc:%s						*/
#define CENO_SIG_OP_ERR                     "004017"		/*信号量OP错误，ID:%d，Desc:%s						*/
#define CENO_RELE_SIG_ERR                   "004018"		/*信号量释放错误，ID:%d，Desc:%s					*/
#define CENO_CRE_PIPE_ERR                   "004019"		/*管道创建错误，名称:%s								*/
#define CENO_OPEN_PIPE_ERR                  "004020"		/*管道打开错误，名称:%s								*/
#define CENO_OCI_TRANSFER_ERR               "002101"		/*OCI调用失败，API名称:%s 用途:%s					*/
#define CENO_SEND_DATA_ERR_I                "005001"		/*发送数据失败,Remote_IP:%s Remote_Port:%d          */
#define CENO_ACCEPT_DATA_ERR_I              "005002"		/*接收数据失败，Local_Port:%d						*/
#define CENO_ACCEPT_DATA_ERR_RILP           "005003"		/*接收数据失败，Remote_IP:%s Local_Port:%d  		*/
#define CENO_CONN_ERR                       "005004"		/*连接失败,Remote_IP:%s Remote_Port:%d   			*/
#define CENO_DISCONN                        "005005"		/*连接异常断开,Remote_IP:%s Remote_Port:%d       	*/
#define CENO_SEND_DATA_ERR_II               "005050"		/*发送数据失败,Remote_IP:%s Remote_Port:%d          */
#define CENO_ACCEPT_DATA_ERR_LILP           "005051"		/*接收数据失败, Local_IP:%s Local_Port:%d			*/
#define CENO_ACCEPT_DATA_ERR_II             "005052"		/*接收数据失败, Local_Port:%d            			*/
#define CENO_SOCKET_CREAT_ERR               "005101"		/*SOCKET创建错误                         			*/
#define CENO_SOCKET_BIND_ERR                "005102"		/*SOCKET端口绑定错误，IP:%s Port:%d      			*/
#define CENO_SOCKET_CONN_ERR                "005103"		/*SOCKET连接错误，IP:%s Port:%u          			*/
#define CENO_SOCKET_SEND_DATA_ERR           "005104"		/*SOCKET 发送数据错误，Remote_IP:%s Remote_Port:%d  */
#define CENO_SOCKET_RECV_DATA_ERR           "005105"		/*SOCKET 接收数据错误，Local_IP:%s Local_Port:%d    */
#define CENO_SOCKET_CLOSE_ERR               "005106"		/*SOCKET 关闭错误                        			*/
#define CENO_SOCKET_CONN_INVALIDATION       "005107"		/*SOCKET 连接失效，Remote_IP:%s Remote_Port:%d      */
#define CENO_SOCKET_DISCONN                 "005108"		/*SOCKET 连接断开，Remote_IP:%s Remote_Port:%d      */
#define CENO_SOCKET_ACCEPT_ERR              "005109"		/*SOCKET 连接接收失败，Remote_IP:%s Remote_Port:%d  */
#define CENO_SOCKET_LISTEN_ERR              "005110"		/*SOCKET 监听失败，IP:%s Port:%d         			*/
#define CENO_SOCKET_CONN_ACHIVEV_UPPERLIMIT "005111"		/*SOCKET 连接数达到上限                  			*/
#define CENO_CORBA_INITIALIZATION_ABNORMITY "005901"		/*CORBA初始化异常                        			*/
#define CENO_CORBA_RELEASE_ABNORMITY        "005902"		/*CORBA释放异常                          			*/
#define CENO_OPERATION_CHAIN_ESTABLISH_ERR  "006001"		/*业务链路建立失败，Remote_IP:%s Remote_Port:%d     */
#define CENO_OPERATION_DATA_SEND_ERR        "006002"		/*业务数据发送失败，Remote_IP:%s Remote_Port:%d     */
#define CENO_OPERATION_DATA_ACCEPT_ERR      "006003"		/*业务数据接收失败，Remote_IP:%s Remote_Port:%d     */
#define CENO_OPERATION_DATA_ERR             "006004"		/*业务数据内容错误                       			*/
#define CENO_OPERATION_CHAIN_OVERTIME       "006005"		/*业务链路建立超时                       			*/
#define CENO_OPERATION_CHAIN_DISCONN        "006006"		/*业务链路异常断开， Desc:%s             			*/

//新添加
//errno
#define CENO_INIT_RESOURCE_LIST             "001908"		/*初始化资源链表失败!*/
#define CENO_INIT_BUFFER_LIST               "001909"		/*初始化缓存链表失败!*/
#define CENO_INIT_REGISTER_LIST				"001910"		/*初始化短信登记链表失败!*/
#define CENO_LOGIN_SMC                      "006007"		/*短信网关认证失败 IP:%s PORT:%d*/
#define CENO_RECVER_MAXTHREADS              "006009"		/*接入线程达到最大值*/ 
#define CENO_DB_LOADDATA_ERR_STR			"002009"		/*%s数据入库%s失败*/
#define CENO_OPERATION_SMTYPE_ERROR_STR		"006010"		/*无法识别的短信类型 Des: %s*/
#define CENO_SMPARSER_ERROR_STR				"006011"		/*短信分析失败, DES: %d*/
#define CENO_DB_MSG_ERROR_STR			    "002005"		/*数据库1、2号信息不一致 ENO:%d %s*/
#define CENO_DB_NOFIND_VALID_ZK_STR		    "002010"		/*没有找到有效主库信息 ENO:%d %s*/
#define CENO_MAIN_FAILED_INIT_LISTEN        "001911"		/*初始化任务接收模块失败*/
#define CENO_MAIN_FAILED_INIT_EXEC          "001912"		/*初始化任务执行模块失败*/
#define	CENO_SOCKET_SELECT_TIMEOUT_R        "005112"		/*SOCKET 等待可读超时，Errno:%d*/
#define CENO_DB_CONN_ERROR_STR2             "002013"		/*数据库连接失败 ENO:%s*/
#define CENO_DB_EXCU_ERROR_STR2             "002014"		/*数据库SQL执行失败 ENO:%s*/
#define CENO_POLICY_REQUEST_FAIL            "006012"		/*%s策略请求失败*/
#define CENO_BUFF_FILEMANFAILED             "001903"		/*磁盘缓存文件管理器创建失败，磁盘缓存功能失效*/
#define CENO_BUFF_FAILEDREAD                "001904"		/*读缓存文件失败*/
#define CENO_ERRDATALEN                     "001905"		/*非法的缓存数据长度*/
#define CENO_BUFF_FAILEDWRITE               "001906"		/*写缓存文件失败*/
#define CENO_DATACHAN_FAILEDCREATEBUFF      "001907"		/*缓冲区创建失败*/
#define CENO_RECVER_FAILEDINITTHREADOBJ     "001944"		/*线程对象初始化失败*/
#define CENO_RECVER_FAILEDRUNTHREAD         "001945"		/*线程对象启动失败*/
#define CENO_MAIN_FAILEDINITDATACHAN        "006013"		/*初始化数据发送通道模块失败*/
#define CENO_FAILED_CREATE_AUTOMAC          "006014"		/*"自动机构造失败，对应策略文件 %s"*/
#define CESTR_FAILED_CREATE_AUTOMAC         "自动机构造失败，对应策略文件 %s"
#define CENO_SEND_DATA_TIMEOUT              "005122"
#define CESTR_SEND_DATA_TIMEOUT             "发送%s数据超时 IP:%s"
#define CENO_SEND_DATA_NOTCPLINK            "005123"
#define CESTR_SEND_DATA_NOTCPLINK           "发送%s数据没有找到对应的链路 IP:%s"


//success
#define CSUC_CREATE_THREAD_SUCCESS			"012002"		/*%s线程创建成功 Thread:%d Pid:%d!*/
#define CSUC_APP_LINK_SUCESS				"012003"		/*业务链路建立成功 IP:%s PORT:%d!*/
#define CSUC_DB_CONN_SUCCESS                "012004"		/*数据库连接成功 Desc:%s*/
#define CENO_DB_FIND_VALID_ZK_STR			"012005"		/*找到有效主库信息 ENO:%d %s*/
#define CENO_POLICY_REQUEST_SUCCESS         "012006"		/*%s策略请求成功*/
#define CSUC_OPERATION_AUTOMACHINE_CRT      "016008"		/*自动机构造成功, 对应策略文件: %s*/
