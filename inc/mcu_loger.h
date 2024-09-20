#if !defined(MCU_LOGER_H)
#define MCU_LOGER_H


#ifdef  __cplusplus
extern "C" {
#endif
#include "mcu_config.h"

#define LOG_USE_VSPRINTF true
#define LOG_USE_COLOR_CODE true
//#include "usart.h"//串口头文件（请替换为你自己的串口驱动的头文件）
extern void UART_Init(unsigned long ulWantedBaud);
extern void usart_puts(char* str);

 #define LOG_PORT_UART_INIT UART_Init(115200)//串口初始化（请替换为你自己实现的串口初始化函数）
 #define LOG_PORT_UART_PUTS(str) usart_puts(str)//串口发送字符串（请替换为你自己实现的串口输出字符串的函数）

//移植这个log组件只需要提供串口初始化和串口输出字符串的接口 

//如果你单片机资源非常紧张也不需要格式的输出
//那么可以设置为flase
//这样的话只会输出添加颜色码后端的字符串fmt，
//可变参不需要传参数，格式化输出的功能都没有输出
//这样无法输出行错误所在行，和rtc时间了

//如果你的log在中断内高速输出或者运行在抢占式的RTOS，
//你裸机可以使用关总中断和开总中断，(还是不建议那样做，开关总中断会受到影响到的事情很多，建议中断不做处理和log输出)
//或者RTOS内使用互斥锁保证日志输出的顺序正确。

#define LOG_LOCK ;
#define LOG_UNLOCK ;

//因为我是裸机运行，且中断内只做判断和变量赋值之类的标记，
//能确保log不在高频率触发的中断进行输出，所以我这里什么锁都不需要加

//如果使用RTC功能需要实现一个函数
//返回当前时间按yyyy-mm-dd hh:mm:ss 格式的字符串
//这里没使用rtc就直接用一个字符串替代一下
#define LOG_RTC_STRING "2001-01-11 00:00:00"

#define LOG_FMT_SIZE 128 //日志格式符缓存大小
#define LOG_BUF_SIZE 256//日志一行数据缓存大小
//注意这2个是函数局部数组变量，占用栈空间
//定义可以按需求改小，但是得太小的话
//可能在显示错误信息的文件和行会占用掉数组能用的空间导致错误信息不全



MCU_ENMU_START(loger_level)

log_level_no_color=0,
log_level_vervose,
log_level_info,
log_level_debug,
log_level_warn,
log_level_erro
MCU_ENMU_END(loger_level)

void mcu_loger_init(void);//日志初始化
void mcu_loger_output(MCU_ENMU_TYPE(loger_level) level ,const char* fmt,...);//日志输出

#define LOG_FILTER_LEVEL log_level_vervose//日志过滤等级

#define log(fmt,...) mcu_loger_output(log_level_no_color,fmt,##__VA_ARGS__)
#define log_v(fmt,...) mcu_loger_output(log_level_vervose,fmt,##__VA_ARGS__)
#define log_i(fmt, ...) mcu_loger_output(log_level_info, fmt, ##__VA_ARGS__)
#define log_d(fmt, ...) mcu_loger_output(log_level_debug, fmt,## __VA_ARGS__)
#define log_w(fmt, ...) mcu_loger_output(log_level_warn, fmt, ##__VA_ARGS__)
#define log_e(fmt, ...) mcu_loger_output(log_level_erro, fmt, ##__VA_ARGS__)

#if LOG_USE_VSPRINTF

//附带警告位置的信息的日志
#define log_w_line(fmt, ...) mcu_loger_output(log_level_warn,__FILE__  " line:%d " fmt, __LINE__,##__VA_ARGS__)

//附带错误位置的信息的日志
#define log_e_mgs(fmt,...) mcu_loger_output(log_level_erro, __FILE__  " line:%d " fmt, __LINE__,##__VA_ARGS__)

//带实时时间的rtc的日志 警告和错误 
#define log_w_rtc_line(fmt,...) mcu_loger_output(log_level_warn, "[%s]" __FILE__  " line:%d " fmt,LOG_RTC_STRING,__LINE__ ,##__VA_ARGS__)
#define log_e_rtc_line(fmt,...) mcu_loger_output(log_level_erro, "[%s]" __FILE__  " line:%d " fmt,LOG_RTC_STRING,__LINE__ ,##__VA_ARGS__)

#else

//附带警告位置的信息的日志
#define log_w_line(fmt, ...) mcu_loger_output(log_level_warn,__FILE__   fmt, __LINE__,##__VA_ARGS__)

//附带错误位置的信息的日志
#define log_e_mgs(fmt,...) mcu_loger_output(log_level_erro, __FILE__   fmt, __LINE__,##__VA_ARGS__)

//带实时时间的rtc的日志 警告和错误 
#define log_w_rtc_line(fmt,...) mcu_loger_output(log_level_warn, LOG_RTC_STRING __FILE__  fmt,##__VA_ARGS__)
#define log_e_rtc_line(fmt,...) mcu_loger_output(log_level_erro, LOG_RTC_STRING __FILE__  fmt,##__VA_ARGS__)


#endif


//颜色转义字符串,需要支持解析颜色转义的串口助手才能看到不然普通的串口助手就是乱码
//如果这些不符合你的习惯可以查对应的终端的颜色转义码来修改
//修改log_output内的log_color数组来定义各个不同的等级的颜色

#define LOG_COLOR_WHITE "\033[37m"//白色）
#define LOG_COLOR_GREEN "\033[32m"//(绿色）
#define LOG_COLOR_BULE "\033[34m"//（蓝色）
#define LOG_COLOR_YELLOW "\033[33m"//（黄色）
#define LOG_COLOR_RED "\033[31m"//（红色）
#define LOG_COLOR_S_RED "\033[5;31m"//（闪烁、红色）





#ifdef  __cplusplus
}
#endif


#endif // MCU_LOGER_H
