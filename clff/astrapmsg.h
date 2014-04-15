
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
/* ����Ӧ�÷��ͳ�ȥ����Ϣ�����͡�                                          */
/* ----------------------------------------------------------------------- */
#include "asmsgtype.h"

/* ----------------------------------------------------------------------- */
/* ���ܣ�                                                      						 */
/*     ��ʼ��trapmsg������������������Ϣ������ʹ�������ļ�ʱ�� 						 */
/*     �ÿյ������ļ���(��conf_filename=NULL)���ñ�������                            						 */
/* ����ֵ��                                                    						 */
/*      0����ʾ��ʼ���ɹ���                                    						 */
/*     -2����ʾָ���������ļ����ɶ���                          						 */
/*     -3����ʾ�����ļ�������"AsTrapMsg"�ֽڿ�ʼ�ġ�           						 */
/*     -4����ʾ�����ļ��б���ָ��TrapProcessNameֵ��           						 */
/*     -5����ʾ�ڴ�������                                  						 */
/* ����˵����                                                  						 */
/*     @conf_filename: ָ�������ļ����ƣ����ÿյ������ļ�������						 */
/*                     ������ʱ������������ȡ��������·����    						 */
/*     @app_identifier: ָ����Ӧ�ñ�ʶ����Ӧ�������ļ�@conf_filename��     */
/*                      �ֽ�����[AsTrapMsg/app_identifier]��   						 */
/* ע����ʹ�ÿյ������ļ�ʱ��ȱʡֻ�����Ϣ��Agent���򣬲������Ϣ��syslog */
/* ----------------------------------------------------------------------- */
extern int trapmsg_init(const char *conf_filename, const char *app_identifier);

/* ----------------------------------------------------------------------- */
/* ���ܣ��ͷ��ڴ���Դ��                                                    */
/* ----------------------------------------------------------------------- */
extern void trapmsg_destory();

/* ----------------------------------------------------------------------- */
/* ���ܣ����ô������syslog��־ѡ�                                    */
/* ----------------------------------------------------------------------- */
extern void trapmsg_set_outto_syslog();

/* ----------------------------------------------------------------------- */
/* ���ܣ����ùر������syslog��־ѡ�                                    */
/* ----------------------------------------------------------------------- */
extern void trapmsg_set_out_notto_syslog();

/* ----------------------------------------------------------------------- */
/* ���ܣ�                                                                  */
/*     ���ָ���������ơ���Ϣ���͡���Ϣ�������־��Ϣ�����ش������      */
/*     �γ���Ϣ��Ҫ�ĸ�ʽ��������trapmsg_init()ָ���������ļ���            */
/* ����ֵ��                                                                */	
/*    >=0����ʾ���͵��ֽ���                                                */	
/*     -1����ʾ�����������ָ��Ϊ�յ����                                  */	
/*     -2����ʾmsgno�г��ȳ��������                                       */
/*     -4����ʾ��va_list��������vsnprintf����ʧ�ܵ����                    */	
/* ����˵����                                                              */	
/*     vmsg_no: ��ʾ��Ϣ���룬ռ5�ַ������ַ���0'                         */
/*     vmsg_xxlx: ��ʾ��Ϣ���� �ַ�����                                    */
/* ----------------------------------------------------------------------- */
extern
int trapmsg (const char *vexename, const char vmsg_xxlx, const char *msgno, ...); 

/* ----------------------------------------------------------------------- */
/* ���ܣ�                                                                  */
/*     ���ָ���������ơ���Ϣ���͡���Ϣ���롢��ʽ��������־��Ϣ�����ش������*/
/* ����ֵ��                                                                */	
/*    >=0����ʾ���͵��ֽ���                                                */	
/*     -1����ʾ�����������ָ��Ϊ�յ����                                  */	
/*     -2����ʾmsgno�г��ȳ��������                                       */	
/*     -3����ʾvfmt�г��ȳ��������                                        */	
/*     -4����ʾ��va_list��������vsnprintf����ʧ�ܵ����                    */	
/* ����˵����                                                              */	
/*     vmsg_no: ��ʾ��Ϣ���룬ռ5�ַ������ַ���0'                         */
/*     vmsg_xxlx: ��ʾ��Ϣ���� �ַ�����                                    */
/* ----------------------------------------------------------------------- */
extern
int trapmsg_fmt (const char *vexename, const char vmsg_xxlx, const char *vmsg_no, const char *vfmt, ...);

/* ----------------------------------------------------------------------- */
/* ���ܣ�                                                                  */
/*     �����Ӧ�ý�����Ϣ��������Ϣ��ʽ������Ϣ                            */
/* ----------------------------------------------------------------------- */
extern void trapmsg_display();

/* ----------------------------------------------------------------------- */
/* ��Ϊѡ��ʽ֮һ: �û��������ʹ��trapmsg()��trapmsg_fmt()��            */
/* ��Ϊѡ��ʽ֮��: �û��������ʹ�����µĺ궨�����򻯺������á�          */

/* ����ʽ������ʽ�����Ϣ��                                                */
extern int trapmsg_start_fmt (const char *format, ...);
extern int trapmsg_stop_fmt (const char *format, ...);
extern int trapmsg_normal_fmt (const char *format, ...);
extern int trapmsg_error_fmt (const char *msg_no, const char *format, ...);
extern int trapmsg_success_fmt (const char *msg_no, const char *format, ...);
extern int trapmsg_status_fmt (const char *msg_no, const char *format, ...);
extern int trapmsg_alarm_fmt (const char *msg_no, const char *format, ...);

/* �޸�ʽ������ʽ�����Ϣ����ʽ�������������ļ���                          */
#define trapmsg_start  trapmsg_start_fmt
#define trapmsg_stop  trapmsg_stop_fmt
#define trapmsg_normal trapmsg_normal_fmt
extern int trapmsg_error(const char *msg_no, ...);
extern int trapmsg_success(const char *msg_no, ...);
extern int trapmsg_status(const char *msg_no, ...);
extern int trapmsg_alarm(const char *msg_no, ...);
/* ----------------------------------------------------------------------- */

/* ��ȡĳ��״̬��Ϣĳ����Ϣ�ŵļ�ʱʱ�� */
extern int get_status_msg_duration(const char *vmsg_no);

#endif
