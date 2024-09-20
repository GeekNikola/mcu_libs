#if !defined(MCU_LIBS_H)
#define MCU_LIBS_H

#include "mcu_config.h"

//调试工具库
//日志库
//mcu_log




//数据结构库
#include "mcu_ring_buf.h"


//状态机协程事件管理库
//#include "mcu_event_manager.h"


//gpio软件模拟TTL硬件协议
//mcu_spi
//mcu_iic
//mcu_xxx


//通用通讯协议
//modbus
//

void mcu_lib_init(void);


#endif // MCU_LIBS_H
