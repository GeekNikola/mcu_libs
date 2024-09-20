#include "mcu_loger.h"
/**
 * @file 文件名： alog.c
 * @author 作者： ageek_nikola  邮箱：(2503865771@qq.com)
 * @brief  功能介绍：alog是一个非常精简的串口输出日志组件，
 *                  类似easyloger，但是比easyloger更简单易用,
 * 
 *                  只有2个实际不到百行的文件，实现了基本日志所需的全部功能。
 * 
 *                  需移植配置的接口选项少，
 *                  实现了串口输出字符串就可以用了，没有C库以外的其他依赖。
 *                  没有存储日志相关的扩展的API
 *                  适合新手使用理解和在资源紧张的单片机上移植使用
 * 
 *                   - 支持可裁剪的格式输出功能
 *                   - 预留互斥锁的调用宏定义
 *                   - 支持自定义log过滤的等级
 *                   - 支持显示报错所在文件和行数
 *                   - 预留RTC功能的宏定义支持输出报错和警告的rtc时间                  
 * 
 * 注意！！！：
 * 推荐在支持颜色码的串口终端查看输出的日志，否则普通串口助手收到的颜色码可能就是乱码
 * 例如：mobaxterm 、xshell 等
 * 
 * @version 版本：0.1
 * @date 日期：2023-09-21
 * 
 * gitee开源地址: https://gitee.com/nikolan/alog.git
 * 
 */


#include "mcu_loger.h"
#include "string.h"

#if LOG_USE_VSPRINTF
#include "stdarg.h"
#include "stdio.h"
#endif


/**
 * @brief  功能介绍： 日志初始化,主要是对日志输出的串口初始化，
 *                   如果有使用rtc功能则可以在这里把rtc也初始化
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
 * @brief  功能介绍：日志输出函数，依赖C库的可变参函数和vsnprintf和strcat和strlen,
 *                   如果单片机资源比较紧张C库占用大可以尝试勾 Use MicroLIB
 *                   
 *                   如果资源还是非常紧张,
 *                   可以不使用格式化输出vsnprintf,的printf类的函数，
 *                   在宏定义LOG_USE_VSPRINTF为false
 *                   直接串口输出字符串不过这样大部分功能都不能使用，
 *                   直接输出加颜色后的fmt字符串,...的可变参数不需要传数据。
 * 
 * @param  参数：level 等级
 * @param  参数：fmt 格式化输出字符串
 * @param  参数：... 格式化数据
 */
void mcu_loger_output(MCU_ENMU_TYPE(loger_level) level ,const char* fmt,...)
{
  //日志颜色字符码数组,如果不喜欢这个样式可以自行修改
	const char* log_color[5]={LOG_COLOR_WHITE,LOG_COLOR_GREEN,LOG_COLOR_BULE,LOG_COLOR_YELLOW,LOG_COLOR_S_RED};
	  //日志缓存字符串
  char log_buf[LOG_BUF_SIZE]={0};
    va_list list;//列表
  LOG_LOCK;//互斥锁上锁

  if(level<LOG_FILTER_LEVEL)//过滤掉日志等级
  return;//低于过滤等级直接返回不输出


  
  #if LOG_USE_VSPRINTF

	char log_fmt[LOG_FMT_SIZE]={0};//格式化字符串
  va_start(list, fmt);//可变参数开始解析

  #endif

  if(LOG_USE_COLOR_CODE&&level>log_level_no_color)//需要颜色时
  {
    #if LOG_USE_VSPRINTF//如果使用格式化输出

    strcat(log_fmt,log_color[level-1]);//加上颜色码前缀
    strcat(log_fmt,fmt);///加上格式字符串
    strcat(log_fmt,"\r\n\033[0m\0");//换回普通颜色码后缀
    vsnprintf(log_buf,sizeof(log_buf), log_fmt, list);//把可变参按格式输出的字符串给到log_buf

    #else
    
    strcat(log_buf,log_color[level-1]);//加颜色码前缀
    strcat(log_buf,fmt);//加上字符串,不用可变参，直接输出字符串
    strcat(log_buf,"\r\n\033[0m\0");//换回普通字符串后缀
    

    #endif
  }else//不需要颜色时
  #if LOG_USE_VSPRINTF

   vsnprintf(log_buf,sizeof(log_buf), fmt, list); //直接输出格式字符不加颜色码
   va_end(list);//格式化字符串解析成功

  #else
  ;

  #endif


#ifndef LOG_PORT_UART_PUTS
#error [port erro]:Please implement the corresponding platforms serial port to send string functions to macro definition LOG_PORT_UART_PUT
#else
LOG_PORT_UART_PUTS(log_buf);//通过串口的输出字符串API来输出日志
#endif
  LOG_UNLOCK;//互斥锁解锁
  
}
