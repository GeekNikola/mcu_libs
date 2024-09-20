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

// 为了调用库方便我们把mcu_ring_buf的api函数宏定义等简写称为mrb做为前缀
//  判断队列是否空:当队头和队尾重合则队列为空
#define mcu_ring_buf_is_empty(mrb) (mrb->front == mrb->rear) // f-r
// 判断队列是否满:当队尾的下一个是队头,则队列为满.取余数是为了让超出的下标回到起点0后重新计算,为了和队列为空区分开，需要保留队尾后的最后一个空间不去使用
#define mcu_ring_buf_is_full(mrb) ((mrb->rear + 1) % (mrb->max_len) == mrb->front) //((r+1)%m)==f
// 获取队列长度:队尾大于队头时，队尾减队头就是长度，但是多次循环后可能出现队尾小于队头这样计算就是负数，这是其实错开了一圈所以需要加上最大长度就保证不是负数，而取余则是没有错开时就截去了不需要加上的最大长度
#define mcu_ring_buf_len(mrb) ((mrb->rear - mrb->front + mrb->max_len) % mrb->max_len) //(r-f+m)%m
// 队列清0回到起点，数据不清空，等下一次直接覆盖原本的值，内存依然占用
// 如果不是使用了malloc，那队列只需要clear就可以了
#define mcu_ring_buf_clear(mrb) (mrb->rear = mrb->front = 0) // r=f=0

// 环形队列动态销毁模板
#define mcu_ring_buf_del(mrb, data) \
    do                              \
    {                               \
        mcu_free(mrb);              \
        mrb = NULL;                 \
        mcu_free(data);             \
        data = NULL;                \
    } while (0)

// 环型队列动态创建模板
#define mcu_ring_buf_new(type, mrb, data, len)                                                                                                        \
    do                                                                                                                                                \
    {                                                                                                                                                 \
        mrb = (MCU_OBJ_TYPE(ring_buf) *)mcu_malloc(sizeof(MCU_OBJ_TYPE(ring_buf)));                                                                   \
        if (NULL != mrb)                                                                                                                              \
        {                                                                                                                                             \
            mrb->max_len = len + 1; /*环形队列为了让满队列和空队列区分开需要留多一个空位看作满队列所以max_len=len+1*/ \
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

// 环形队列入队模板(没有满就正常入队)
#define mcu_ring_buf_enqueue(mrb, data, val)                                \
    do                                                                      \
    {                                                                       \
        if (!mcu_ring_buf_is_full(mrb))                                     \
        {                                                                   \
            data[mrb->rear] = val;                      /*对数据赋值*/ \
            mrb->rear = (mrb->rear + 1) % mrb->max_len; /*队尾移动*/    \
        }                                                                   \
        else                                                                \
        {                                                                   \
            log_w_rtc_line("is full cannot enqueue");                       \
        }                                                                   \
    } while (0)

// 环形队列取队头模板(不为空则返回队头数据否则返回0)
#define mcu_ring_buf_front(mrb, type, data) \
    (type)((!mcu_ring_buf_is_empty(mrb)) ? data[mrb->front] : (log_w_rtc_line("is empty return 0"), 0))

// 环形队列取队尾模板(不为空则返回队尾数据否则返回0)
#define mcu_ring_buf_rear(mrb, type, data) \
    (type)((!mcu_ring_buf_is_empty(mrb)) ? data[(mrb->rear - 1 + mrb->max_len) % mrb->max_len] : (log_w_rtc_line("is empty return 0"), 0))

// 环形队列出队模板(不为空则正常出队，队列为空则返回特殊值并输出警告)
#define mcu_ring_buf_dequeue(mrb) \
    mrb->front = (!mcu_ring_buf_is_empty(mrb)) ? ((mrb->front + 1) % mrb->max_len) : (log_w_rtc_line("is empty return 0"), mrb->front)

// 环形队列查询值模板(查看从队头开始偏移多少位的数据，不出队)
#define mcu_ring_buf_peek(mrb, type, data, pos) \
    (type)(((pos >= 0 && pos < mcu_ring_buf_len(mrb)) ? data[(mrb->front + pos) % mrb->max_len] : (log_w_rtc_line("invalid pos returning 0"),0)))

// 环形队列的循环遍历,(注意index和rear变量作用域)
#define mcu_ring_buf_forech(mrb, index) \
    for (uint16_t index = mrb->front; index != mrb->rear; index = (index + 1) % mrb->max_len)

#ifdef __cplusplus
}
#endif

#endif // mcu_ring_buf
