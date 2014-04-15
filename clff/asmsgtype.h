
#ifndef __ASMSGTYPE_H__
#define __ASMSGTYPE_H__


/* 定义消息类型 字符常量
	 消息类型使用1字节字符
	 ‘0’--运行、‘1’--错误、‘2’--成功、
	 ‘3’--状态、‘4’--告警、‘5’--启动、
	 ‘6’--退出
*/
#define MASTrap_MSGCLASS_Run        '0'
#define MASTrap_MSGCLASS_Error      '1'
#define MASTrap_MSGCLASS_Success    '2'
#define MASTrap_MSGCLASS_Status     '3'
#define MASTrap_MSGCLASS_Alarm      '4'
#define MASTrap_MSGCLASS_Start      '5'
#define MASTrap_MSGCLASS_Stop       '6'


#endif
