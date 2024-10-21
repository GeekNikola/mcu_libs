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

  mcu_lock; // ��Ҫ�����жϷ�ֹ��������� �����жϲ��������ж�����ʧЧ�������ݻ�����

  MCU_OBJ_TYPE(event) *e = (MCU_OBJ_TYPE(event) *)mcu_malloc(sizeof(MCU_OBJ_TYPE(event)));
  e->step = 1;
  e->await_us = 0;
  e->callback = callback;
  e->is_repeatable = is_repeatable;
  e->args = args;

  mcu_list_push_back(em->events[priority], e, sizeof(MCU_OBJ_TYPE(event)));

  if (em->priority > priority) // ������¼������ȼ��������¼����ȼ���ʱ
    em->priority = priority;   // �ص������ȼ�

  // �������ȼ��¼��鱻����
  // �����ȼ��¼���һ������ѯ���������ó�ʹ��Ȩ

  // ���ص������ȼ��¼�����ѯ
  // ���������ȼ��¼�û�м�ʱ��Ӧ
  // ���ǲ���������ռִ��

  // ��������ȼ��¼��ڵ������ڵ��������ִ��
  mcu_unlock;
  return e;
}

// 1us��ʱ���ж�,��1ms�����¼��ĵ���
// ���и����ȼ���ռ�ҵ����ȼ��¼�һ�������Ѿ�ִ����ɣ����л��¼�����
// ������ȼ��򲻹ܵ����ȼ��¼������Ƿ����ִ�ж�ֱ���ڵ��ȵ�1ms�ж�ֱ��ִ�У����ǲ���ռ��̫�á�
void mcu_em_tick(void)
{

  if (!(mcu_em->await_us++ % MCU_EVENTS_SCHED_CYCLE))
  {

    mcu_lock; // ��Ҫ�����жϷ�ֹ��������� �����жϲ��������ж�����ʧЧ�������ݻ�����

    // ������ȼ��¼�����ֱ�����ж�ִ�� ��ռʽ
    // ������ռ��ʱ��̫�ã��ر��жϣ���Ӱ��mcu_em->await_us�ļ����жϵ��������������ʱ�����Ӻ�
    // ����ֻ�ʺ��ر�������¼� �����⵽���ϡ���Ҫ��ʱ�رջ�����ִ�и߼�ָ���жϡ� ��ʱ�����¼���Э����ʱ��Ҳ����Ҫ��
    if (!mcu_list_is_empty(mcu_em->events[0])) // ֱ��ִ����
    {
	   mcu_em->priority = 0; // ��һ�����������Ͳ��ܻص������ȼ�
       mcu_em->pos = 0;
      // �¼���֮��ʹ������protothread��Э�̵���
      mcu_list_forech(mcu_em->events[ mcu_em->priority], node)
      {

        MCU_OBJ_TYPE(event) *e = node->data;
        e->callback(e);
      }
    }
    else
    {

      // ��ʱ������ѯ��ǰ�Ƿ���������ȼ��¼�
      for (uint16_t priority = 0; priority < mcu_em->priority; priority++)
      {

        if (!mcu_list_is_empty(mcu_em->events[priority]))
        {

          mcu_em->priority = priority; // ��һ�����������Ͳ��ܻص������ȼ�
          mcu_em->pos = 0;
          mcu_em->find_higher_priority = 1;//�ҵ��������ȼ��¼�ִ�и����ȼ��¼�����

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
      // �¼���֮��ʹ������protothread��Э�̵���
      mcu_em->pos = 0;
      // 1~n֮��Э�̣��������������С�
      mcu_list_forech(mcu_em->events[mcu_em->priority], node)
      {
        if (mcu_em->find_higher_priority)//����ҵ��������ȼ��¼�ִ�и����ȼ��¼�����

        {
          break;//�˳���ǰ���ȼ��¼�����ı���
        }
		
        MCU_OBJ_TYPE(event) *e = node->data;
        e->callback(e);
        mcu_em->pos++;
		
      }
	    
	   if (mcu_em->find_higher_priority)//�ҵ��������ȼ��¼�
      {
      
      mcu_em->find_higher_priority = 0;
	  continue;//�˳�����ѭ��,������ִ��,���ǵ�ǰ���ȼ��¼����������ɣ����Ǳ��������ȼ��жϵĻ������ȼ�����++
		  
      }else if(mcu_em->priority < MCU_EVENTS_PRIORITY_MAX)
      mcu_em->priority++;//��ǰ���ȼ��¼������Ѿ�����һ���ֵ��������ȼ��¼�����
	  
         
  }
}
