#include "mcu_events.h"

//有序链表
//创建时插入
//不管是插入还是删除都不能在中断内触发
//但是不使用中断触发的话也不能等太久，可以在调度前检测
//默认定义一个空闲任务做状态灯闪烁


//如果是双链表可以按优先级来决定在当前事件前还是后来寻找位置插入

//没有唤醒或者定时时间到，就不重新遍历

//-负数优先级是事件的休眠
//插入时需要判断优先级绝对值



uint8_t eid_idle;

EVENT_CALL_BACK(idle)
{
  CSM_EVENT_START

  log_d("idle called\n");
  CSM_EVENT_STEP_TO_NXET

  CSM_EVENT_END

}

bool mcu_events_queue_create(MCU_OBJ_TYPE(ring_buf)** rb,MCU_OBJ_TYPE(event)** event)
{
   // mcu_ring_buf_new((MCU_OBJ_TYPE(ring_buf),rb,)

   mcu_ring_buf_new(MCU_OBJ_TYPE(event),(*rb),(*event),CFG_EVENTS_CNT_MAX);

return rb!=NULL;
	
}

uint8_t mcu_events_add_event(MCU_OBJ_TYPE(event_manager) em,event_call_back_t call_back,int8_t priority)
{
    MCU_OBJ_TYPE(event) event;
    uint8_t eid;

for(em.priority=CFG_EVENTS_PRIORITY_MAX-1;em.priority>=0;em.priority--)
    {
        mcu_ring_buf_forech(em.rqs[em.priority],em.eid)
        {
              if((em.events[em.priority])[em.eid].step!=0)
                {
                
              //这里添加事件时先遍历已经存在队列的运行中的事件，按回调函数区分是否上同一个事件
              //如果是同一个事件则不重复触发和添加，防止事件的多次处理
              //TODO:不过不知道会不会有需要同一个事件重复触发重复处理的需求,这个后面再考虑
                if((em.events[em.priority])[em.eid].call_back==call_back)
                {
                  printf("event_ready_exist\n");
                return em.eid;
                }
          
                }
        }
    }

    printf("add new event\n");
    event.state=event_no_ready;
    event.step=1;//进入步骤1则开始执行事件处理
    event.wait_us=0;
    event.call_back=call_back;

    eid= em.rqs[priority]->rear;//入队后队尾就会改变了,入队前先取得当前队尾作为eid
 
    mcu_ring_buf_enqueue_T((em.rqs[priority]),(em.events[priority]),event);  
    printf("add new event ok\n");  
    return eid;


}

bool em_init(obj_event_manager_t* em )
{


    em->eid=0;   
    
    for(em->priority=CFG_EVENTS_PRIORITY_MAX-1;em->priority>=0;em->priority--)
    {
      printf("p:%d \n",em->priority);
      mcu_events_queue_create(&em->rqs[em->priority],&em->events[em->priority]);

      if(em->rqs[em->priority])
      printf("rq create\n");
	  else
		  return false;

      if(em->events[em->priority])
      printf("event create\n");
	  	  else
		  return false;

    }


    printf("add idle event\n");
    eid_idle=mcu_events_add_event(*em,call_back_idle,0);
    printf("add idle event ok eid_idle=%d\n",eid_idle);
	
	return true;
}

void em_event_loop_start(obj_event_manager_t* em)
{
      
    //   get_stick;//stick (uint32_t)
    //    event_wait-= diff_stick;
    while(1)
    {
    // printf("usleep\n");
      //delay_us(1000000);


    for(em->priority=CFG_EVENTS_PRIORITY_MAX-1;em->priority>=0;em->priority--)
    {
        // mcu_events_queue_create(&em->rqs[em->priority],&em->events[em->priority]);
    // (em.events[em.priority])[em.eid].step
    mcu_ring_buf_forech(em->rqs[em->priority],em->eid)
    {
        printf("p:%d e:%d\n",em->priority,em->eid);
      if((em->events[em->priority])[em->eid].wait_us>0)
            {
              
      
           log_i("eid:%d wait:%dus\n",em->eid,(em->events[em->priority])[em->eid].wait_us);

              (em->events[em->priority])[em->eid].wait_us--;
            } 
       if((em->events[em->priority])[em->eid].state==event_no_ready)
            (em->events[em->priority])[em->eid].state=event_ready;
    }

    mcu_ring_buf_forech(em->rqs[em->priority],em->eid)
    {

            log_i("p:%d e:%d\n",em->priority,em->eid);

          if((em->events[em->priority])[em->eid].step>0)
            {

         
            log_i("eid:%d step:%d \n",em->eid,(em->events[em->priority])[em->eid].step);
            (em->events[em->priority])[em->eid].call_back(em);
      
            }

                // if((em->events[em->priority])[em->eid].call_back==call_back_idle)
                // {
                //   printf("have event_idle\n");
                // // return em.eid;
                // }

    }

    }

    }

}






