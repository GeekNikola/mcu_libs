#if !defined(MCU_EVENTS_H)
#define MCU_EVENTS_H


#include "mcu_config.h"
#include "mcu_loger.h"
#include "mcu_ring_buf.h"

#define CFG_EVENTS_CNT_MAX 6
#define CFG_EVENTS_PRIORITY_MAX 6
#define CFG_EVENTS_IDLE_FOREVER_RUNING 1





typedef uint16_t step_t;



 struct obj_event_manager ;

typedef step_t (*event_call_back_t)(struct obj_event_manager*);


MCU_ENMU_START(event_state)
    event_no_ready,
    event_ready,
    event_runing,
    event_stop,
    event_done
MCU_ENMU_END(event_state)


MCU_OBJ_START(event)

MCU_ENMU_TYPE(event_state) state;
step_t step;
uint32_t wait_us;
event_call_back_t call_back;



MCU_OBJ_END(event)


//MCU_OBJ_START(event_manager)
MCU_OBJ_START(event_manager)
uint8_t eid;
int8_t priority;//负数优先级是被挂起的
MCU_OBJ_TYPE(event)* events[CFG_EVENTS_CNT_MAX]; 
MCU_OBJ_TYPE(ring_buf)* rqs[CFG_EVENTS_CNT_MAX];


MCU_OBJ_END(event_manager)

#define EVENT_CALL_BACK(name) step_t call_back_##name(MCU_OBJ_TYPE(event_manager)* em)

	
	
extern uint8_t eid_idle;

bool   mcu_events_queue_create(MCU_OBJ_TYPE(ring_buf)** rq,MCU_OBJ_TYPE(event)** event);
uint8_t mcu_events_add_event(MCU_OBJ_TYPE(event_manager) em,event_call_back_t call_back,int8_t priority);
bool em_init(MCU_OBJ_TYPE(event_manager)* em );
void em_event_loop_start(MCU_OBJ_TYPE(event_manager)* em);
	
/**
 * @brief  功能介绍：事件开始执行
 */
#define CSM_EVENT_START      \
    switch ((em->events[em->priority])[em->eid].step) \
    {                        \
    case 1:                  \
        (em->events[em->priority])[em->eid].step = 1;


/**
 * @brief  功能介绍：事件步骤分割
 */
#define CSM_EVENT_STEP_TO_NXET      \
    (em->events[em->priority])[em->eid].step = __LINE__ - 1; \
    break;                          \
    case __LINE__:



/**
 * @brief  功能介绍：事件步骤等待
 */
#define CSM_EVENT_STEP_TO_WAIT(us)  \
     (em->events[em->priority])[em->eid].wait_us = us;        \
    (em->events[em->priority])[em->eid].step = __LINE__ - 1; \
    break;                          \
    case __LINE__:                  \
        if ( (em->events[em->priority])[em->eid].wait_us > 0) \
        {                           \
            break;                  \
        }



/**
 * @brief  功能介绍：事件停止执行
 */
#define CSM_EVENT_STOP \
     (em->events[em->priority])[em->eid].step = 0;




/**
 * @brief  功能介绍：事件完成结束运行
 */
#define CSM_EVENT_END                                                 \
     (em->events[em->priority])[em->eid].step = 0;                    \
    break;                                                            \
    default:                                                          \
        break;                                                        \
        }                                                             \
        if ( (em->events[em->priority])[em->eid].wait_us == 0)                                  \
        {                                                             \
            if ( (em->events[em->priority])[em->eid].step != 0 &&  (em->events[em->priority])[em->eid].step < __LINE__)   \
                 (em->events[em->priority])[em->eid].step++;                                    \
            else if (em->eid == eid_idle) \
                 (em->events[em->priority])[em->eid].step = CFG_EVENTS_IDLE_FOREVER_RUNING;      \
            else{                                                      \
                 (em->events[em->priority])[em->eid].step = 0;                                  \
                 log_i("exit!!!\n");\
                mcu_ring_buf_dequeue_T(em->rqs[em->priority]);     \
            }\
        }\
        return  (em->events[em->priority])[em->eid].step;\






 













#endif // MCU_EVENTS_H
