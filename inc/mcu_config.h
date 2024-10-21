#if !defined(MCU_CONFIG_H)
#define MCU_CONFIG_H

//公共标准库头文件
#include "stdint.h"//包含对uint8_t等类型的定义
#include "stdbool.h"//包含bool 布尔类型定义
#include "stdio.h"//使用printf 重定义
#include "assert.h"


#include "tlsf.h"//引入tlsf（Two-Level Segregated Fit memory allocator implementation）库（https://github.com/mattconte/tlsf）
#ifdef INCLUDED_tlsf//如果库被引入了就把动态内存分配的函数改为使用tlsf库来实现

extern tlsf_t  mcu_memory;

#define mcu_malloc(size)  tlsf_malloc(mcu_memory,size)
#define mcu_realloc(size) tlsf_realloc (mcu_memory,size)
#define mcu_free(ptr)  tlsf_free(mcu_memory,ptr)

#else//如果没有引入则使用默认的标准库的动态内存分配来实现

#include "stdlib.h"

#define mcu_malloc  malloc
#define mcu_realloc realloc 
#define mcu_free  free


#endif // INCLUDED_tlsf


#define mcu_lock __disable_irq();  /* 禁止全局中断*/
#define mcu_unlock __enable_irq();    /*  使能全局中断 */

// __set_PRIMASK(1)  /* 禁止全局中断*/
// __set_PRIMASK(0)  /*  使能全局中断 */





//为了强制定义结构体必须使用typedef重新命名，同时新类型名称以obj_开头，以_t结尾,提供以下宏来定义结构体对象
#define MCU_OBJ_START(name) typedef struct obj_##name {
#define MCU_OBJ_END(name) }obj_##name##_t;
#define MCU_OBJ_TYPE(name) obj_##name##_t
#define MCU_OBJ_PREV(name) typedef struct obj_##name obj_##name##_t

/**
 * @brief 如何使用？宏参数name 是结构体对象名称 obj_name_t，把结构体成员放入中间,MCU_OBJ_START的后面,MCU_OBJ_END的前面
 * 
MCU_OBJ_START(ring_buf)
    uint32_t max_len;
    uint32_t front;
    uint32_t rear;
MCU_OBJ_END(ring_buf)
 * 
 */

#define MCU_ENMU_START(name)typedef enum enum_##name {
#define MCU_ENMU_END(name) }enum_##name##_t;
#define MCU_ENMU_TYPE(name) enum_##name##_t





//typedef enum mcu_erro_id 
//{

//mcu_erro_invalid_param,//参数不可用
//mcu_erro_timeout,//超时,通讯失败
//mcu_erro_memory,//内存错误,内存不够分配或者越界
//mcu_erro_undefine//未定义错误

//}mcu_erro_id_t;


//#define mcu_erro_id_printf(id)\
//do{
//      
////mcu_log

//}while (0)

#define MCU_MEMORY_POOL_SIZE 8192


#endif // MCU_CONFIG_H
