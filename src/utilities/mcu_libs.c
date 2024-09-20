#include "mcu_libs.h"

#ifdef INCLUDED_tlsf//����ⱻ�����˾ͰѶ�̬�ڴ����ĺ�����Ϊʹ��tlsf����ʵ��

tlsf_t  mcu_memory;
uint8_t mcu_memory_pool [MCU_MEMORY_POOL_SIZE];

void mcu_memory_init(void) {

    // ��ʼ�� TLSF �ڴ������
    mcu_memory = tlsf_create_with_pool(mcu_memory_pool,sizeof(mcu_memory_pool));

}



void mcu_lib_init(void)
{

mcu_memory_init();

}


void __aeabi_assert(const char *expr, const char *file, int line) {
    log_e_rtc_line("Assertion failed: %s, file %s, line %d\n", expr, file, line);
   while(1);  // ֹͣ����
}

#endif
