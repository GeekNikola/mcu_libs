#if !defined(MCU_CONFIG_H)
#define MCU_CONFIG_H

//������׼��ͷ�ļ�
#include "stdint.h"//������uint8_t�����͵Ķ���
#include "stdbool.h"//����bool �������Ͷ���
#include "stdio.h"//ʹ��printf �ض���
#include "assert.h"


#include "tlsf.h"//����tlsf��Two-Level Segregated Fit memory allocator implementation���⣨https://github.com/mattconte/tlsf��
#ifdef INCLUDED_tlsf//����ⱻ�����˾ͰѶ�̬�ڴ����ĺ�����Ϊʹ��tlsf����ʵ��

extern tlsf_t  mcu_memory;

#define mcu_malloc(size)  tlsf_malloc(mcu_memory,size)
#define mcu_realloc(size) tlsf_realloc (mcu_memory,size)
#define mcu_free(ptr)  tlsf_free(mcu_memory,ptr)

#else//���û��������ʹ��Ĭ�ϵı�׼��Ķ�̬�ڴ������ʵ��

#include "stdlib.h"

#define mcu_malloc  malloc
#define mcu_realloc realloc 
#define mcu_free  free


#endif // INCLUDED_tlsf


#define mcu_lock __disable_irq();  /* ��ֹȫ���ж�*/
#define mcu_unlock __enable_irq();    /*  ʹ��ȫ���ж� */

// __set_PRIMASK(1)  /* ��ֹȫ���ж�*/
// __set_PRIMASK(0)  /*  ʹ��ȫ���ж� */





//Ϊ��ǿ�ƶ���ṹ�����ʹ��typedef����������ͬʱ������������obj_��ͷ����_t��β,�ṩ���º�������ṹ�����
#define MCU_OBJ_START(name) typedef struct obj_##name {
#define MCU_OBJ_END(name) }obj_##name##_t;
#define MCU_OBJ_TYPE(name) obj_##name##_t
#define MCU_OBJ_PREV(name) typedef struct obj_##name obj_##name##_t

/**
 * @brief ���ʹ�ã������name �ǽṹ��������� obj_name_t���ѽṹ���Ա�����м�,MCU_OBJ_START�ĺ���,MCU_OBJ_END��ǰ��
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

//mcu_erro_invalid_param,//����������
//mcu_erro_timeout,//��ʱ,ͨѶʧ��
//mcu_erro_memory,//�ڴ����,�ڴ治���������Խ��
//mcu_erro_undefine//δ�������

//}mcu_erro_id_t;


//#define mcu_erro_id_printf(id)\
//do{
//      
////mcu_log

//}while (0)

#define MCU_MEMORY_POOL_SIZE 8192


#endif // MCU_CONFIG_H
