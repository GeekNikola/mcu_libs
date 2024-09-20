#include "mcu_events.h"

//��������
//����ʱ����
//�����ǲ��뻹��ɾ�����������ж��ڴ���
//���ǲ�ʹ���жϴ����Ļ�Ҳ���ܵ�̫�ã������ڵ���ǰ���
//Ĭ�϶���һ������������״̬����˸


//�����˫������԰����ȼ��������ڵ�ǰ�¼�ǰ���Ǻ���Ѱ��λ�ò���

//û�л��ѻ��߶�ʱʱ�䵽���Ͳ����±���

//-�������ȼ����¼�������
//����ʱ��Ҫ�ж����ȼ�����ֵ



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
                
              //��������¼�ʱ�ȱ����Ѿ����ڶ��е������е��¼������ص����������Ƿ���ͬһ���¼�
              //�����ͬһ���¼����ظ���������ӣ���ֹ�¼��Ķ�δ���
              //TODO:������֪���᲻������Ҫͬһ���¼��ظ������ظ����������,��������ٿ���
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
    event.step=1;//���벽��1��ʼִ���¼�����
    event.wait_us=0;
    event.call_back=call_back;

    eid= em.rqs[priority]->rear;//��Ӻ��β�ͻ�ı���,���ǰ��ȡ�õ�ǰ��β��Ϊeid
 
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






