#if !defined(MCU_EVENTS_H)

#define MCU_EVENTS_H

#include "mcu_config.h"
#include "mcu_libs.h"
#include "mcu_loger.h"

#define MCU_EVENTS_CNT_MAX 6
#define MCU_EVENTS_PRIORITY_BASE(level) level + 3 // 优先级不低于3个,一个最高优先级，一个最低优先级
#define MCU_EVENTS_IDLE_FOREVER_RUNING 1

#define MCU_EVENTS_PRIORITY_MAX MCU_EVENTS_PRIORITY_BASE(1)

#define MCU_EVENTS_SCHED_CYCLE 1000 // us

// 最高优先级级别事件组
//... ...
// 最低优先级基本事件组

/*


（如果占内存事件完成后就释放？事件函数的变量在创建时new，事件结束后del？）
对如果变量还需要保留说明事件不能被结束，只能让出等待，结束时应该在最后一步del变量）
list是事件协程组列表
node的data是事件本身，包含事件处理函数指针和事件中需要使用的变量和

del操作到底是在事件内执行还是等空闲再做？空闲时如何寻找需要释放的事件？


{

调度器操作：
处理事件插入,消息传递等,一般不做其他处理
0,[]->[]->[]定时触发，（最高优先级调度事件）
//优先级0的事件分步遍历执行如果放在定时器中断，是不是能作为比较高优先级处理
//优先级0事件插入是立即插入直接执行，不归调度器管理？



事件插入时需要判断事件是否存在，或者说事件存在的次数？
如果有些事件只能触发一次，正在处理是不触发则事件如果已经存在则不创建。
怎么判断事件触发次数？遍历找太慢了（好像不太行除非有全局变量）

1,[]->[]->[]->[]//尾插链表事件协程组(事件特别紧急可以头插或者放0级别做强制调度事件处理)



n,[]-[]-[]//空闲时做空闲事件的释放？
//GC


}

 */

// 有序链表
// 创建时插入
// 不管是插入还是删除都不能在中断内触发
// 但是不使用中断触发的话也不能等太久，可以在调度前检测
// 默认定义一个空闲任务做状态灯闪烁

// 如果是双链表可以按优先级来决定在当前事件前还是后来寻找位置插入
// 为了省内存还是使用单链表好
// 但是事件管理最好保存当前node的前继节点，方便删除或者插入

// 插入时需要判断优先级绝对值

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
void *args;//保留

MCU_OBJ_END(event)



#define MCU_EVENT_CALL_BACK(_event_name_) step_t event_##_event_name_##_callback(MCU_OBJ_TYPE(event) * e)
extern obj_event_manager_t* mcu_em;


void mcu_event_manager_init(MCU_OBJ_TYPE(event_manager)* em);
MCU_OBJ_TYPE(event) * mcu_events_add_event(MCU_OBJ_TYPE(event_manager) * em, event_callback_t callback, uint8_t priority, uint8_t is_repeatable, void *args);
void mcu_em_tick(void);
void mcu_event_loop(void);


/**
 * @brief  功能介绍：事件开始执行
 */
#define MCU_EVENT_START \
    switch (e->step)    \
    {                   \
    case 1:             \
        e->step = 1;

/**
 * @brief  功能介绍：事件步骤分割
 */
#define MCU_EVENT_STEP_TO_NXET \
    e->step = __LINE__ - 1;    \
    break;                     \
    case __LINE__:

/**
 * @brief  功能介绍：事件步骤等待
 * 系统计数值 em->await_us定时1us就+1
 * 期待计数值 e->await_us
 * 延时值 us 需要限定us不超过31位0xfffffffe
 *
 * 没有到期待计数值时case直接break不往下执行下一步
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
         if ((e->await_us > us && mcu_em->await_us < e->await_us) ||                               /*期待计数值大于延时数 是计数值没有溢出 只要没到计数值就return就行*/                                                                                                  \
              (e->await_us < us && (mcu_em->await_us >= 0xFFFFFFFE || mcu_em->await_us < e->await_us))) /*期待计数值小于延时数 是计数值溢出 这个情况下需要在大于最大延时，小于期待计数值之间return指定超过期待计数值*/                                                           \
            return  e->step;

/**
 * @brief  功能介绍：事件停止执行
 */
#define MCU_EVENT_STOP \
    e->step = 0;

/**
 * @brief  功能介绍：事件完成结束运行
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
            /*log_i("exit!!! 是需要执行list的del_node?\n"); */\
            }                                                 \
        return e->step;

#endif // MCU_EVENTS_H
