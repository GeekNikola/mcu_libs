#include "mcu_libs.h"

#ifdef INCLUDED_tlsf//如果库被引入了就把动态内存分配的函数改为使用tlsf库来实现

tlsf_t  mcu_memory;
uint8_t mcu_memory_pool [MCU_MEMORY_POOL_SIZE];

void mcu_memory_init(void) {

    // 初始化 TLSF 内存分配器
    mcu_memory = tlsf_create_with_pool(mcu_memory_pool,sizeof(mcu_memory_pool));

}



void mcu_lib_init(void)
{

mcu_memory_init();

}


void __aeabi_assert(const char *expr, const char *file, int line) {
    log_e_rtc_line("Assertion failed: %s, file %s, line %d\n", expr, file, line);
   while(1);  // 停止程序
}

#endif
