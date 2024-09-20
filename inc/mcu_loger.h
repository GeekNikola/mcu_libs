#if !defined(MCU_LOGER_H)
#define MCU_LOGER_H


#ifdef  __cplusplus
extern "C" {
#endif
#include "mcu_config.h"

#define LOG_USE_VSPRINTF true
#define LOG_USE_COLOR_CODE true
//#include "usart.h"//����ͷ�ļ������滻Ϊ���Լ��Ĵ���������ͷ�ļ���
extern void UART_Init(unsigned long ulWantedBaud);
extern void usart_puts(char* str);

 #define LOG_PORT_UART_INIT UART_Init(115200)//���ڳ�ʼ�������滻Ϊ���Լ�ʵ�ֵĴ��ڳ�ʼ��������
 #define LOG_PORT_UART_PUTS(str) usart_puts(str)//���ڷ����ַ��������滻Ϊ���Լ�ʵ�ֵĴ�������ַ����ĺ�����

//��ֲ���log���ֻ��Ҫ�ṩ���ڳ�ʼ���ʹ�������ַ����Ľӿ� 

//����㵥Ƭ����Դ�ǳ�����Ҳ����Ҫ��ʽ�����
//��ô��������Ϊflase
//�����Ļ�ֻ����������ɫ���˵��ַ���fmt��
//�ɱ�β���Ҫ����������ʽ������Ĺ��ܶ�û�����
//�����޷�����д��������У���rtcʱ����

//������log���ж��ڸ������������������ռʽ��RTOS��
//���������ʹ�ù����жϺͿ����жϣ�(���ǲ��������������������жϻ��ܵ�Ӱ�쵽������ܶ࣬�����жϲ��������log���)
//����RTOS��ʹ�û�������֤��־�����˳����ȷ��

#define LOG_LOCK ;
#define LOG_UNLOCK ;

//��Ϊ����������У����ж���ֻ���жϺͱ�����ֵ֮��ı�ǣ�
//��ȷ��log���ڸ�Ƶ�ʴ������жϽ������������������ʲô��������Ҫ��

//���ʹ��RTC������Ҫʵ��һ������
//���ص�ǰʱ�䰴yyyy-mm-dd hh:mm:ss ��ʽ���ַ���
//����ûʹ��rtc��ֱ����һ���ַ������һ��
#define LOG_RTC_STRING "2001-01-11 00:00:00"

#define LOG_FMT_SIZE 128 //��־��ʽ�������С
#define LOG_BUF_SIZE 256//��־һ�����ݻ����С
//ע����2���Ǻ����ֲ����������ռ��ջ�ռ�
//������԰������С�����ǵ�̫С�Ļ�
//��������ʾ������Ϣ���ļ����л�ռ�õ��������õĿռ䵼�´�����Ϣ��ȫ



MCU_ENMU_START(loger_level)

log_level_no_color=0,
log_level_vervose,
log_level_info,
log_level_debug,
log_level_warn,
log_level_erro
MCU_ENMU_END(loger_level)

void mcu_loger_init(void);//��־��ʼ��
void mcu_loger_output(MCU_ENMU_TYPE(loger_level) level ,const char* fmt,...);//��־���

#define LOG_FILTER_LEVEL log_level_vervose//��־���˵ȼ�

#define log(fmt,...) mcu_loger_output(log_level_no_color,fmt,##__VA_ARGS__)
#define log_v(fmt,...) mcu_loger_output(log_level_vervose,fmt,##__VA_ARGS__)
#define log_i(fmt, ...) mcu_loger_output(log_level_info, fmt, ##__VA_ARGS__)
#define log_d(fmt, ...) mcu_loger_output(log_level_debug, fmt,## __VA_ARGS__)
#define log_w(fmt, ...) mcu_loger_output(log_level_warn, fmt, ##__VA_ARGS__)
#define log_e(fmt, ...) mcu_loger_output(log_level_erro, fmt, ##__VA_ARGS__)

#if LOG_USE_VSPRINTF

//��������λ�õ���Ϣ����־
#define log_w_line(fmt, ...) mcu_loger_output(log_level_warn,__FILE__  " line:%d " fmt, __LINE__,##__VA_ARGS__)

//��������λ�õ���Ϣ����־
#define log_e_mgs(fmt,...) mcu_loger_output(log_level_erro, __FILE__  " line:%d " fmt, __LINE__,##__VA_ARGS__)

//��ʵʱʱ���rtc����־ ����ʹ��� 
#define log_w_rtc_line(fmt,...) mcu_loger_output(log_level_warn, "[%s]" __FILE__  " line:%d " fmt,LOG_RTC_STRING,__LINE__ ,##__VA_ARGS__)
#define log_e_rtc_line(fmt,...) mcu_loger_output(log_level_erro, "[%s]" __FILE__  " line:%d " fmt,LOG_RTC_STRING,__LINE__ ,##__VA_ARGS__)

#else

//��������λ�õ���Ϣ����־
#define log_w_line(fmt, ...) mcu_loger_output(log_level_warn,__FILE__   fmt, __LINE__,##__VA_ARGS__)

//��������λ�õ���Ϣ����־
#define log_e_mgs(fmt,...) mcu_loger_output(log_level_erro, __FILE__   fmt, __LINE__,##__VA_ARGS__)

//��ʵʱʱ���rtc����־ ����ʹ��� 
#define log_w_rtc_line(fmt,...) mcu_loger_output(log_level_warn, LOG_RTC_STRING __FILE__  fmt,##__VA_ARGS__)
#define log_e_rtc_line(fmt,...) mcu_loger_output(log_level_erro, LOG_RTC_STRING __FILE__  fmt,##__VA_ARGS__)


#endif


//��ɫת���ַ���,��Ҫ֧�ֽ�����ɫת��Ĵ������ֲ��ܿ�����Ȼ��ͨ�Ĵ������־�������
//�����Щ���������ϰ�߿��Բ��Ӧ���ն˵���ɫת�������޸�
//�޸�log_output�ڵ�log_color���������������ͬ�ĵȼ�����ɫ

#define LOG_COLOR_WHITE "\033[37m"//��ɫ��
#define LOG_COLOR_GREEN "\033[32m"//(��ɫ��
#define LOG_COLOR_BULE "\033[34m"//����ɫ��
#define LOG_COLOR_YELLOW "\033[33m"//����ɫ��
#define LOG_COLOR_RED "\033[31m"//����ɫ��
#define LOG_COLOR_S_RED "\033[5;31m"//����˸����ɫ��





#ifdef  __cplusplus
}
#endif


#endif // MCU_LOGER_H
