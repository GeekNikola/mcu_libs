#if !defined(mcu_ring_buf)
#define mcu_ring_buf

#ifdef __cplusplus
extern "C"
{
#endif

#include "mcu_config.h"
#include "mcu_loger.h"

    MCU_OBJ_START(ring_buf)

    uint16_t max_len;
    uint16_t front;
    uint16_t rear;

    MCU_OBJ_END(ring_buf)

// Ϊ�˵��ÿⷽ�����ǰ�mcu_ring_buf��api�����궨��ȼ�д��Ϊmrb��Ϊǰ׺
//  �ж϶����Ƿ��:����ͷ�Ͷ�β�غ������Ϊ��
#define mcu_ring_buf_is_empty(mrb) (mrb->front == mrb->rear) // f-r
// �ж϶����Ƿ���:����β����һ���Ƕ�ͷ,�����Ϊ��.ȡ������Ϊ���ó������±�ص����0�����¼���,Ϊ�˺Ͷ���Ϊ�����ֿ�����Ҫ������β������һ���ռ䲻ȥʹ��
#define mcu_ring_buf_is_full(mrb) ((mrb->rear + 1) % (mrb->max_len) == mrb->front) //((r+1)%m)==f
// ��ȡ���г���:��β���ڶ�ͷʱ����β����ͷ���ǳ��ȣ����Ƕ��ѭ������ܳ��ֶ�βС�ڶ�ͷ����������Ǹ�����������ʵ����һȦ������Ҫ������󳤶Ⱦͱ�֤���Ǹ�������ȡ������û�д�ʱ�ͽ�ȥ�˲���Ҫ���ϵ���󳤶�
#define mcu_ring_buf_len(mrb) ((mrb->rear - mrb->front + mrb->max_len) % mrb->max_len) //(r-f+m)%m
// ������0�ص���㣬���ݲ���գ�����һ��ֱ�Ӹ���ԭ����ֵ���ڴ���Ȼռ��
// �������ʹ����malloc���Ƕ���ֻ��Ҫclear�Ϳ�����
#define mcu_ring_buf_clear(mrb) (mrb->rear = mrb->front = 0) // r=f=0

// ���ζ��ж�̬����ģ��
#define mcu_ring_buf_del(mrb, data) \
    do                              \
    {                               \
        mcu_free(mrb);              \
        mrb = NULL;                 \
        mcu_free(data);             \
        data = NULL;                \
    } while (0)

// ���Ͷ��ж�̬����ģ��
#define mcu_ring_buf_new(type, mrb, data, len)                                                                                                        \
    do                                                                                                                                                \
    {                                                                                                                                                 \
        mrb = (MCU_OBJ_TYPE(ring_buf) *)mcu_malloc(sizeof(MCU_OBJ_TYPE(ring_buf)));                                                                   \
        if (NULL != mrb)                                                                                                                              \
        {                                                                                                                                             \
            mrb->max_len = len + 1; /*���ζ���Ϊ���������кͿն������ֿ���Ҫ����һ����λ��������������max_len=len+1*/ \
            mcu_ring_buf_clear(mrb);                                                                                                                  \
            if (NULL == (data = (type *)mcu_malloc(sizeof(type) * (len + 1))))                                                                        \
            {                                                                                                                                         \
                log_e_rtc_line("data new fail!");                                                                                                     \
                mcu_ring_buf_del(mrb, data);                                                                                                          \
            }                                                                                                                                         \
            else                                                                                                                                      \
                break;                                                                                                                                \
        }                                                                                                                                             \
        log_e_rtc_line("ringbuf new fail!");                                                                                                          \
    } while (0)

// ���ζ������ģ��(û�������������)
#define mcu_ring_buf_enqueue(mrb, data, val)                                \
    do                                                                      \
    {                                                                       \
        if (!mcu_ring_buf_is_full(mrb))                                     \
        {                                                                   \
            data[mrb->rear] = val;                      /*�����ݸ�ֵ*/ \
            mrb->rear = (mrb->rear + 1) % mrb->max_len; /*��β�ƶ�*/    \
        }                                                                   \
        else                                                                \
        {                                                                   \
            log_w_rtc_line("is full cannot enqueue");                       \
        }                                                                   \
    } while (0)

// ���ζ���ȡ��ͷģ��(��Ϊ���򷵻ض�ͷ���ݷ��򷵻�0)
#define mcu_ring_buf_front(mrb, type, data) \
    (type)((!mcu_ring_buf_is_empty(mrb)) ? data[mrb->front] : (log_w_rtc_line("is empty return 0"), 0))

// ���ζ���ȡ��βģ��(��Ϊ���򷵻ض�β���ݷ��򷵻�0)
#define mcu_ring_buf_rear(mrb, type, data) \
    (type)((!mcu_ring_buf_is_empty(mrb)) ? data[(mrb->rear - 1 + mrb->max_len) % mrb->max_len] : (log_w_rtc_line("is empty return 0"), 0))

// ���ζ��г���ģ��(��Ϊ�����������ӣ�����Ϊ���򷵻�����ֵ���������)
#define mcu_ring_buf_dequeue(mrb) \
    mrb->front = (!mcu_ring_buf_is_empty(mrb)) ? ((mrb->front + 1) % mrb->max_len) : (log_w_rtc_line("is empty return 0"), mrb->front)

// ���ζ��в�ѯֵģ��(�鿴�Ӷ�ͷ��ʼƫ�ƶ���λ�����ݣ�������)
#define mcu_ring_buf_peek(mrb, type, data, pos) \
    (type)(((pos >= 0 && pos < mcu_ring_buf_len(mrb)) ? data[(mrb->front + pos) % mrb->max_len] : (log_w_rtc_line("invalid pos returning 0"),0)))

// ���ζ��е�ѭ������,(ע��index��rear����������)
#define mcu_ring_buf_forech(mrb, index) \
    for (uint16_t index = mrb->front; index != mrb->rear; index = (index + 1) % mrb->max_len)

#ifdef __cplusplus
}
#endif

#endif // mcu_ring_buf
