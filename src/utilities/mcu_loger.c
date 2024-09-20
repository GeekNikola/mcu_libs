#include "mcu_loger.h"
/**
 * @file �ļ����� alog.c
 * @author ���ߣ� ageek_nikola  ���䣺(2503865771@qq.com)
 * @brief  ���ܽ��ܣ�alog��һ���ǳ�����Ĵ��������־�����
 *                  ����easyloger�����Ǳ�easyloger��������,
 * 
 *                  ֻ��2��ʵ�ʲ������е��ļ���ʵ���˻�����־�����ȫ�����ܡ�
 * 
 *                  ����ֲ���õĽӿ�ѡ���٣�
 *                  ʵ���˴�������ַ����Ϳ������ˣ�û��C�����������������
 *                  û�д洢��־��ص���չ��API
 *                  �ʺ�����ʹ����������Դ���ŵĵ�Ƭ������ֲʹ��
 * 
 *                   - ֧�ֿɲü��ĸ�ʽ�������
 *                   - Ԥ���������ĵ��ú궨��
 *                   - ֧���Զ���log���˵ĵȼ�
 *                   - ֧����ʾ���������ļ�������
 *                   - Ԥ��RTC���ܵĺ궨��֧���������;����rtcʱ��                  
 * 
 * ע�⣡������
 * �Ƽ���֧����ɫ��Ĵ����ն˲鿴�������־��������ͨ���������յ�����ɫ����ܾ�������
 * ���磺mobaxterm ��xshell ��
 * 
 * @version �汾��0.1
 * @date ���ڣ�2023-09-21
 * 
 * gitee��Դ��ַ: https://gitee.com/nikolan/alog.git
 * 
 */


#include "mcu_loger.h"
#include "string.h"

#if LOG_USE_VSPRINTF
#include "stdarg.h"
#include "stdio.h"
#endif


/**
 * @brief  ���ܽ��ܣ� ��־��ʼ��,��Ҫ�Ƕ���־����Ĵ��ڳ�ʼ����
 *                   �����ʹ��rtc����������������rtcҲ��ʼ��
 */
void mcu_loger_init(void)
{


#ifndef LOG_PORT_UART_INIT
#error [port erro]: Please implement the serial port initialization function for the corresponding platform to define LOG_PORT_UART_INIT for the macro
#else
LOG_PORT_UART_INIT;
#endif

}

/**
 * @brief  ���ܽ��ܣ���־�������������C��Ŀɱ�κ�����vsnprintf��strcat��strlen,
 *                   �����Ƭ����Դ�ȽϽ���C��ռ�ô���Գ��Թ� Use MicroLIB
 *                   
 *                   �����Դ���Ƿǳ�����,
 *                   ���Բ�ʹ�ø�ʽ�����vsnprintf,��printf��ĺ�����
 *                   �ں궨��LOG_USE_VSPRINTFΪfalse
 *                   ֱ�Ӵ�������ַ������������󲿷ֹ��ܶ�����ʹ�ã�
 *                   ֱ���������ɫ���fmt�ַ���,...�Ŀɱ��������Ҫ�����ݡ�
 * 
 * @param  ������level �ȼ�
 * @param  ������fmt ��ʽ������ַ���
 * @param  ������... ��ʽ������
 */
void mcu_loger_output(MCU_ENMU_TYPE(loger_level) level ,const char* fmt,...)
{
  //��־��ɫ�ַ�������,�����ϲ�������ʽ���������޸�
	const char* log_color[5]={LOG_COLOR_WHITE,LOG_COLOR_GREEN,LOG_COLOR_BULE,LOG_COLOR_YELLOW,LOG_COLOR_S_RED};
	  //��־�����ַ���
  char log_buf[LOG_BUF_SIZE]={0};
    va_list list;//�б�
  LOG_LOCK;//����������

  if(level<LOG_FILTER_LEVEL)//���˵���־�ȼ�
  return;//���ڹ��˵ȼ�ֱ�ӷ��ز����


  
  #if LOG_USE_VSPRINTF

	char log_fmt[LOG_FMT_SIZE]={0};//��ʽ���ַ���
  va_start(list, fmt);//�ɱ������ʼ����

  #endif

  if(LOG_USE_COLOR_CODE&&level>log_level_no_color)//��Ҫ��ɫʱ
  {
    #if LOG_USE_VSPRINTF//���ʹ�ø�ʽ�����

    strcat(log_fmt,log_color[level-1]);//������ɫ��ǰ׺
    strcat(log_fmt,fmt);///���ϸ�ʽ�ַ���
    strcat(log_fmt,"\r\n\033[0m\0");//������ͨ��ɫ���׺
    vsnprintf(log_buf,sizeof(log_buf), log_fmt, list);//�ѿɱ�ΰ���ʽ������ַ�������log_buf

    #else
    
    strcat(log_buf,log_color[level-1]);//����ɫ��ǰ׺
    strcat(log_buf,fmt);//�����ַ���,���ÿɱ�Σ�ֱ������ַ���
    strcat(log_buf,"\r\n\033[0m\0");//������ͨ�ַ�����׺
    

    #endif
  }else//����Ҫ��ɫʱ
  #if LOG_USE_VSPRINTF

   vsnprintf(log_buf,sizeof(log_buf), fmt, list); //ֱ�������ʽ�ַ�������ɫ��
   va_end(list);//��ʽ���ַ��������ɹ�

  #else
  ;

  #endif


#ifndef LOG_PORT_UART_PUTS
#error [port erro]:Please implement the corresponding platforms serial port to send string functions to macro definition LOG_PORT_UART_PUT
#else
LOG_PORT_UART_PUTS(log_buf);//ͨ�����ڵ�����ַ���API�������־
#endif
  LOG_UNLOCK;//����������
  
}
