
/**
 File: astrapmsg.h
 Version: 1.0.0.0
 Create Date: 2007/07/16
 Author: YinJianBo
 Description: 
*/

#ifndef __Astrapmsg_v4_H__
#define __Astrapmsg_v4_H__

/* ----------------------------------------------------------------------- */
/* 定义应用发送出去的消息的类型。                                          */
/* ----------------------------------------------------------------------- */
#include "asmsgtype.h"

/* ----------------------------------------------------------------------- */
/* 功能：                                                      						 */
/*     初始化trapmsg函数库依赖的配置信息，当不使用配置文件时， 						 */
/*     用空的配置文件名(即conf_filename=NULL)调用本函数。                            						 */
/* 返回值：                                                    						 */
/*      0：表示初始化成功。                                    						 */
/*     -2：表示指定的配置文件不可读。                          						 */
/*     -3：表示配置文件不是以"AsTrapMsg"分节开始的。           						 */
/*     -4：表示配置文件中必须指定TrapProcessName值。           						 */
/*     -5：表示内存分配错误。                                  						 */
/* 参数说明：                                                  						 */
/*     @conf_filename: 指定配置文件名称，当用空的配置文件名调用						 */
/*                     本函数时函数库自主获取进程名、路径。    						 */
/*     @app_identifier: 指定本应用标识。对应到配置文件@conf_filename的     */
/*                      分节名称[AsTrapMsg/app_identifier]。   						 */
/* 注：当使用空的配置文件时，缺省只输出信息到Agent程序，不输出信息给syslog */
/* ----------------------------------------------------------------------- */
extern int trapmsg_init(const char *conf_filename, const char *app_identifier);

/* ----------------------------------------------------------------------- */
/* 功能：释放内存资源。                                                    */
/* ----------------------------------------------------------------------- */
extern void trapmsg_destory();

/* ----------------------------------------------------------------------- */
/* 功能：设置打开输出到syslog日志选项。                                    */
/* ----------------------------------------------------------------------- */
extern void trapmsg_set_outto_syslog();

/* ----------------------------------------------------------------------- */
/* 功能：设置关闭输出到syslog日志选项。                                    */
/* ----------------------------------------------------------------------- */
extern void trapmsg_set_out_notto_syslog();

/* ----------------------------------------------------------------------- */
/* 功能：                                                                  */
/*     输出指定进程名称、消息类型、消息编码的日志消息给本地代理程序。      */
/*     形成消息需要的格式化串来自trapmsg_init()指定的配置文件。            */
/* 返回值：                                                                */	
/*    >=0：表示发送的字节数                                                */	
/*     -1：表示输入参数中有指针为空的情况                                  */	
/*     -2：表示msgno有长度超长的情况                                       */
/*     -4：表示用va_list方法调用vsnprintf函数失败的情况                    */	
/* 参数说明：                                                              */	
/*     vmsg_no: 表示消息编码，占5字符，左补字符’0'                         */
/*     vmsg_xxlx: 表示消息类型 字符常量                                    */
/* ----------------------------------------------------------------------- */
extern
int trapmsg (const char *vexename, const char vmsg_xxlx, const char *msgno, ...); 

/* ----------------------------------------------------------------------- */
/* 功能：                                                                  */
/*     输出指定进程名称、消息类型、消息编码、格式化串的日志消息给本地代理程序。*/
/* 返回值：                                                                */	
/*    >=0：表示发送的字节数                                                */	
/*     -1：表示输入参数中有指针为空的情况                                  */	
/*     -2：表示msgno有长度超长的情况                                       */	
/*     -3：表示vfmt有长度超长的情况                                        */	
/*     -4：表示用va_list方法调用vsnprintf函数失败的情况                    */	
/* 参数说明：                                                              */	
/*     vmsg_no: 表示消息编码，占5字符，左补字符’0'                         */
/*     vmsg_xxlx: 表示消息类型 字符常量                                    */
/* ----------------------------------------------------------------------- */
extern
int trapmsg_fmt (const char *vexename, const char vmsg_xxlx, const char *vmsg_no, const char *vfmt, ...);

/* ----------------------------------------------------------------------- */
/* 功能：                                                                  */
/*     输出：应用进程信息、各个消息格式串的信息                            */
/* ----------------------------------------------------------------------- */
extern void trapmsg_display();

/* ----------------------------------------------------------------------- */
/* 作为选择方式之一: 用户程序可以使用trapmsg()和trapmsg_fmt()。            */
/* 作为选择方式之二: 用户程序可以使用以下的宏定义来简化函数调用。          */

/* 带格式化串方式输出信息。                                                */
extern int trapmsg_start_fmt (const char *format, ...);
extern int trapmsg_stop_fmt (const char *format, ...);
extern int trapmsg_normal_fmt (const char *format, ...);
extern int trapmsg_error_fmt (const char *msg_no, const char *format, ...);
extern int trapmsg_success_fmt (const char *msg_no, const char *format, ...);
extern int trapmsg_status_fmt (const char *msg_no, const char *format, ...);
extern int trapmsg_alarm_fmt (const char *msg_no, const char *format, ...);

/* 无格式化串方式输出信息，格式化串来自配置文件。                          */
#define trapmsg_start  trapmsg_start_fmt
#define trapmsg_stop  trapmsg_stop_fmt
#define trapmsg_normal trapmsg_normal_fmt
extern int trapmsg_error(const char *msg_no, ...);
extern int trapmsg_success(const char *msg_no, ...);
extern int trapmsg_status(const char *msg_no, ...);
extern int trapmsg_alarm(const char *msg_no, ...);
/* ----------------------------------------------------------------------- */

/* 获取某个状态消息某个消息号的计时时长 */
extern int get_status_msg_duration(const char *vmsg_no);

#endif
