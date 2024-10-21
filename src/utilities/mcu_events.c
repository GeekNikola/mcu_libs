#include "mcu_events.h"

obj_event_manager_t mcu_event_manager;
obj_event_manager_t *mcu_em = &mcu_event_manager;

MCU_OBJ_TYPE(event) * idle_e;

MCU_EVENT_CALL_BACK(idle)
{

  MCU_EVENT_START

  log_d("idle called 1\n");

  uint32_t a = 9999999;
  while (a--)
    ;

  // MCU_EVENT_STEP_AWAIT(5000);

  log_d("idle called 2\n");
  a = 9999999;
  while (a--)
    ;
  // MCU_EVENT_STEP_AWAIT(5000);

  MCU_EVENT_END
}

MCU_EVENT_CALL_BACK(idle2)
{

  MCU_EVENT_START

  log_d("idle2 called 1\n");
  MCU_EVENT_STEP_AWAIT(10000);

  log_d("idle2 called 2\n");
  MCU_EVENT_STEP_AWAIT(10000);

  MCU_EVENT_END
}

void mcu_event_manager_init(MCU_OBJ_TYPE(event_manager) * em)
{

  for (uint16_t i = 0; i < MCU_EVENTS_PRIORITY_MAX; i++)
  {

    mcu_list_new(em->events[i], "event");
  }

  em->priority = 0;
  em->pos = 0;

  idle_e = mcu_events_add_event(em, event_idle_callback, MCU_EVENTS_PRIORITY_MAX - 1, 1, NULL);
  idle_e = mcu_events_add_event(em, event_idle2_callback, MCU_EVENTS_PRIORITY_MAX - 2, 1, NULL);
}

MCU_OBJ_TYPE(event) * mcu_events_add_event(MCU_OBJ_TYPE(event_manager) * em, event_callback_t callback, uint8_t priority, uint8_t is_repeatable, void *args)
{

  mcu_lock; // 需要屏蔽中断防止创建被打断 屏蔽中断并不会让中断请求失效，而是暂缓处理

  MCU_OBJ_TYPE(event) *e = (MCU_OBJ_TYPE(event) *)mcu_malloc(sizeof(MCU_OBJ_TYPE(event)));
  e->step = 1;
  e->await_us = 0;
  e->callback = callback;
  e->is_repeatable = is_repeatable;
  e->args = args;

  mcu_list_push_back(em->events[priority], e, sizeof(MCU_OBJ_TYPE(event)));

  if (em->priority > priority) // 新添加事件的优先级比现在事件优先级高时
    em->priority = priority;   // 回到高优先级

  // 当高优先级事件组被唤醒
  // 低优先级事件组一旦在轮询过程主动让出使用权

  // 则会回到高优先级事件组轮询
  // 这避免高优先级事件没有及时响应
  // 但是并非立即抢占执行

  // 而最高优先级事件在调度周期到达后立即执行
  mcu_unlock;
  return e;
}

// 1us定时器中断,到1ms触发事件的调度
// 若有高优先级抢占且低优先级事件一个步骤已经执行完成，则切换事件链表
// 最高优先级则不管低优先级事件步骤是否完成执行都直接在调度的1ms中断直接执行，但是不能占用太久。
void mcu_em_tick(void)
{

  if (!(mcu_em->await_us++ % MCU_EVENTS_SCHED_CYCLE))
  {

    mcu_lock; // 需要屏蔽中断防止创建被打断 屏蔽中断并不会让中断请求失效，而是暂缓处理

    // 最高优先级事件允许直接在中断执行 抢占式
    // 但是抢占的时间太久，关闭中断，会影响mcu_em->await_us的计数中断导致其他任务的延时计数延后
    // 所以只适合特别紧急的事件 例如检测到故障、需要定时关闭机器、执行高级指令中断、 这时其他事件的协程延时了也不重要了
    if (!mcu_list_is_empty(mcu_em->events[0])) // 直到执行完
    {
	   mcu_em->priority = 0; // 在一个步骤结束后就才能回到高优先级
       mcu_em->pos = 0;
      // 事件组之间使用类似protothread的协程调度
      mcu_list_forech(mcu_em->events[ mcu_em->priority], node)
      {

        MCU_OBJ_TYPE(event) *e = node->data;
        e->callback(e);
      }
    }
    else
    {

      // 定时遍历查询当前是否是最高优先级事件
      for (uint16_t priority = 0; priority < mcu_em->priority; priority++)
      {

        if (!mcu_list_is_empty(mcu_em->events[priority]))
        {

          mcu_em->priority = priority; // 在一个步骤结束后就才能回到高优先级
          mcu_em->pos = 0;
          mcu_em->find_higher_priority = 1;//找到更高优先级事件执行高优先级事件链表

          break;
        }
      }

     
    }
	 mcu_unlock;
	
  }
}

void mcu_event_loop(void)
{

  while (1)
  {
      // 事件组之间使用类似protothread的协程调度
      mcu_em->pos = 0;
      // 1~n之间协程，步骤内阻塞运行。
      mcu_list_forech(mcu_em->events[mcu_em->priority], node)
      {
        if (mcu_em->find_higher_priority)//如果找到更高优先级事件执行高优先级事件链表

        {
          break;//退出当前优先级事件链表的遍历
        }
		
        MCU_OBJ_TYPE(event) *e = node->data;
        e->callback(e);
        mcu_em->pos++;
		
      }
	    
	   if (mcu_em->find_higher_priority)//找到更改优先级事件
      {
      
      mcu_em->find_higher_priority = 0;
	  continue;//退出本次循环,不继续执行,不是当前优先级事件链表遍历完成，而是被更高优先级中断的话，优先级不能++
		  
      }else if(mcu_em->priority < MCU_EVENTS_PRIORITY_MAX)
      mcu_em->priority++;//当前优先级事件链表已经遍历一次轮到更低优先级事件链表
	  
         
  }
}
