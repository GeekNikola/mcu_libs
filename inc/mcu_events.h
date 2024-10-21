#if !defined(MCU_EVENTS_H)

#define MCU_EVENTS_H

#include "mcu_config.h"
#include "mcu_libs.h"
#include "mcu_loger.h"

#define MCU_EVENTS_CNT_MAX 6
#define MCU_EVENTS_PRIORITY_BASE(level) level + 3 // ���ȼ�������3��,һ��������ȼ���һ��������ȼ�
#define MCU_EVENTS_IDLE_FOREVER_RUNING 1

#define MCU_EVENTS_PRIORITY_MAX MCU_EVENTS_PRIORITY_BASE(1)

#define MCU_EVENTS_SCHED_CYCLE 1000 // us

// ������ȼ������¼���
//... ...
// ������ȼ������¼���

/*


�����ռ�ڴ��¼���ɺ���ͷţ��¼������ı����ڴ���ʱnew���¼�������del����
�������������Ҫ����˵���¼����ܱ�������ֻ���ó��ȴ�������ʱӦ�������һ��del������
list���¼�Э�����б�
node��data���¼����������¼�������ָ����¼�����Ҫʹ�õı�����

del�������������¼���ִ�л��ǵȿ�������������ʱ���Ѱ����Ҫ�ͷŵ��¼���


{

������������
�����¼�����,��Ϣ���ݵ�,һ�㲻����������
0,[]->[]->[]��ʱ��������������ȼ������¼���
//���ȼ�0���¼��ֲ�����ִ��������ڶ�ʱ���жϣ��ǲ�������Ϊ�Ƚϸ����ȼ�����
//���ȼ�0�¼���������������ֱ��ִ�У��������������



�¼�����ʱ��Ҫ�ж��¼��Ƿ���ڣ�����˵�¼����ڵĴ�����
�����Щ�¼�ֻ�ܴ���һ�Σ����ڴ����ǲ��������¼�����Ѿ������򲻴�����
��ô�ж��¼�����������������̫���ˣ�����̫�г�����ȫ�ֱ�����

1,[]->[]->[]->[]//β�������¼�Э����(�¼��ر��������ͷ����߷�0������ǿ�Ƶ����¼�����)



n,[]-[]-[]//����ʱ�������¼����ͷţ�
//GC


}

 */

// ��������
// ����ʱ����
// �����ǲ��뻹��ɾ�����������ж��ڴ���
// ���ǲ�ʹ���жϴ����Ļ�Ҳ���ܵ�̫�ã������ڵ���ǰ���
// Ĭ�϶���һ������������״̬����˸

// �����˫������԰����ȼ��������ڵ�ǰ�¼�ǰ���Ǻ���Ѱ��λ�ò���
// Ϊ��ʡ�ڴ滹��ʹ�õ������
// �����¼�������ñ��浱ǰnode��ǰ�̽ڵ㣬����ɾ�����߲���

// ����ʱ��Ҫ�ж����ȼ�����ֵ

// MCU_OBJ_START(event_manager)

typedef uint16_t step_t;



MCU_OBJ_START(event_manager)

uint32_t await_us;
uint8_t pos;
uint8_t priority;
uint8_t find_higher_priority;
MCU_OBJ_TYPE(list) * events[MCU_EVENTS_PRIORITY_MAX];

MCU_OBJ_END(event_manager)


MCU_OBJ_PREV(event);
typedef step_t (*event_callback_t)(MCU_OBJ_TYPE(event) * e);

MCU_OBJ_START(event)

step_t step;
volatile uint32_t await_us;
uint8_t is_repeatable;
event_callback_t callback;
void *args;//����

MCU_OBJ_END(event)



#define MCU_EVENT_CALL_BACK(_event_name_) step_t event_##_event_name_##_callback(MCU_OBJ_TYPE(event) * e)
extern obj_event_manager_t* mcu_em;


void mcu_event_manager_init(MCU_OBJ_TYPE(event_manager)* em);
MCU_OBJ_TYPE(event) * mcu_events_add_event(MCU_OBJ_TYPE(event_manager) * em, event_callback_t callback, uint8_t priority, uint8_t is_repeatable, void *args);
void mcu_em_tick(void);
void mcu_event_loop(void);


/**
 * @brief  ���ܽ��ܣ��¼���ʼִ��
 */
#define MCU_EVENT_START \
    switch (e->step)    \
    {                   \
    case 1:             \
        e->step = 1;

/**
 * @brief  ���ܽ��ܣ��¼�����ָ�
 */
#define MCU_EVENT_STEP_TO_NXET \
    e->step = __LINE__ - 1;    \
    break;                     \
    case __LINE__:

/**
 * @brief  ���ܽ��ܣ��¼�����ȴ�
 * ϵͳ����ֵ em->await_us��ʱ1us��+1
 * �ڴ�����ֵ e->await_us
 * ��ʱֵ us ��Ҫ�޶�us������31λ0xfffffffe
 *
 * û�е��ڴ�����ֵʱcaseֱ��break������ִ����һ��
 *
 *
 */
#define MCU_EVENT_STEP_AWAIT(us)                                                                                                                                                                                                                               \
   /* if (us >= 0xFFFFFFFE)                                                                                                                                                                                                                                      \
        log_e("us > max_us(0xFFFFFFFE)") */                                                                                                                                                                                                                      \
            e->await_us = (uint32_t)(mcu_em->await_us + (us));                                                                                                                                                                                                     \
    e->step = __LINE__ - 1;                                                                                                                                                                                                                                    \
    break;                                                                                                                                                                                                                                                     \
    case __LINE__:                                                                                                                                                                                                                                             \
         if ((e->await_us > us && mcu_em->await_us < e->await_us) ||                               /*�ڴ�����ֵ������ʱ�� �Ǽ���ֵû����� ֻҪû������ֵ��return����*/                                                                                                  \
              (e->await_us < us && (mcu_em->await_us >= 0xFFFFFFFE || mcu_em->await_us < e->await_us))) /*�ڴ�����ֵС����ʱ�� �Ǽ���ֵ��� ����������Ҫ�ڴ��������ʱ��С���ڴ�����ֵ֮��returnָ�������ڴ�����ֵ*/                                                           \
            return  e->step;

/**
 * @brief  ���ܽ��ܣ��¼�ִֹͣ��
 */
#define MCU_EVENT_STOP \
    e->step = 0;

/**
 * @brief  ���ܽ��ܣ��¼���ɽ�������
 */
#define MCU_EVENT_END                                         \
    e->step = 0;                                              \
    break;                                                    \
    default:                                                  \
        break;                                                \
        }                                                     \
            if (e->step != 0 && e->step < __LINE__)           \
                e->step++;                                    \
            else if (e->is_repeatable)                        \
                e->step = 1;                                  \
            else                                              \
            {                                                 \
                e->step = 0;                                  \
            /*log_i("exit!!! ����Ҫִ��list��del_node?\n"); */\
            }                                                 \
        return e->step;

#endif // MCU_EVENTS_H
