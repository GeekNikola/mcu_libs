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
    //ring_buf 不包含void* data 这是因为想要使用中括号[]来取值的话，指针需要强制转到对应类型才能正确使用
    //这会导致所有操作都是出现data的地方都要需要void*对应type*
    //如果包含data缓冲区数据，则泛型data的操作必须每次都借助第三方局部变量或者强制转换表达式结果来取值，
//与其加一堆强转，还不如让data和ringbuf分离,这样宏定义除了在创建以外都不需要填type，避免转换错误和转换消耗的时间

//而 list 的node节点无法完全分离data ，所以需要void*,这带来一些问题，但是同时也更加灵活，因为node和node之间类型都可以不一样

// 为了调用库方便我们把mcu_ring_buf的api函数宏定义等简写称为mrb做为前缀
//  判断队列是否空:当队头和队尾重合则队列为空
#define mcu_ring_buf_is_empty(_mrb_) (_mrb_->front == _mrb_->rear) // f-r
// 判断队列是否满:当队尾的下一个是队头,则队列为满.取余数是为了让超出的下标回到起点0后重新计算,为了和队列为空区分开，需要保留队尾后的最后一个空间不去使用
#define mcu_ring_buf_is_full(_mrb_) ((_mrb_->rear + 1) % (_mrb_->max_len) == _mrb_->front) //((r+1)%m)==f
// 获取队列长度:队尾大于队头时，队尾减队头就是长度，但是多次循环后可能出现队尾小于队头这样计算就是负数，这是其实错开了一圈所以需要加上最大长度就保证不是负数，而取余则是没有错开时就截去了不需要加上的最大长度
#define mcu_ring_buf_len(_mrb_) ((_mrb_->rear - _mrb_->front + _mrb_->max_len) % _mrb_->max_len) //(r-f+m)%m
// 队列清0回到起点，数据不清空，等下一次直接覆盖原本的值，内存依然占用
// 如果不是使用了malloc，那队列只需要clear就可以了
#define mcu_ring_buf_clear(_mrb_) (_mrb_->rear = _mrb_->front = 0) // r=f=0

// 环形队列动态销毁模板
#define mcu_ring_buf_del(_mrb_, _data_) \
    do                                  \
    {                                   \
        mcu_free(_data_);               \
        _data_ = NULL;                  \
        mcu_free(_mrb_);                \
        _mrb_ = NULL;                   \
    } while (0)

// 环型队列动态创建模板
#define mcu_ring_buf_new(_type_, _mrb_, _data_, _len_)                                                                                                    \
    do                                                                                                                                                    \
    {                                                                                                                                                     \
        _mrb_ = (MCU_OBJ_TYPE(ring_buf) *)mcu_malloc(sizeof(MCU_OBJ_TYPE(ring_buf)));                                                                     \
        if (NULL != _mrb_)                                                                                                                                \
        {                                                                                                                                                 \
            _mrb_->max_len = _len_ + 1; /*环形队列为了让满队列和空队列区分开需要留多一个空位看作满队列所以max_len=len+1*/ \
            mcu_ring_buf_clear(_mrb_);                                                                                                                    \
            if (NULL == (_data_ = (_type_ *)mcu_malloc(sizeof(_type_) * (_len_ + 1))))                                                                    \
            {                                                                                                                                             \
                log_e_rtc_line("data new fail!");                                                                                                         \
                mcu_ring_buf_del(_mrb_, _data_);                                                                                                          \
            }                                                                                                                                             \
            else                                                                                                                                          \
                break;                                                                                                                                    \
        }                                                                                                                                                 \
        log_e_rtc_line("ringbuf new fail!");                                                                                                              \
    } while (0)

// 环形队列入队模板(没有满就正常入队)
#define mcu_ring_buf_enqueue(_mrb_, _data_, _val_)                                \
    do                                                                            \
    {                                                                             \
        if (!mcu_ring_buf_is_full(_mrb_))                                         \
        {                                                                         \
            data[_mrb_->rear] = _val_;                        /*对数据赋值*/ \
            _mrb_->rear = (_mrb_->rear + 1) % _mrb_->max_len; /*队尾移动*/    \
        }                                                                         \
        else                                                                      \
        {                                                                         \
            log_w_rtc_line("is full cannot enqueue");                             \
        }                                                                         \
    } while (0)

// 环形队列取队头模板(不为空则返回队头数据否则返回0)
#define mcu_ring_buf_front(_mrb_, _data_) \
  ((!mcu_ring_buf_is_empty(_mrb_)) ? _data_[_mrb_->front] : (log_w_rtc_line("is empty return 0"), 0))

// 环形队列取队尾模板(不为空则返回队尾数据否则返回0)
#define mcu_ring_buf_rear(_mrb_, _data_) \
    ((!mcu_ring_buf_is_empty(_mrb_)) ? _data_[(_mrb_->rear - 1 + _mrb_->max_len) % _mrb_->max_len] : (log_w_rtc_line("is empty return 0"), 0))

// 环形队列出队模板(不为空则正常出队，队列为空则返回特殊值并输出警告)
#define mcu_ring_buf_dequeue(_mrb_) \
    _mrb_->front = (!mcu_ring_buf_is_empty(_mrb_)) ? ((_mrb_->front + 1) % _mrb_->max_len) : (log_w_rtc_line("is empty return 0"), _mrb_->front)

// 环形队列查询值模板(查看从队头开始偏移多少位的数据，不出队)
#define mcu_ring_buf_peek(_mrb_, _data_, _pos_) \
    (((_pos_ >= 0 && _pos_ < mcu_ring_buf_len(_mrb_)) ? data[(_mrb_->front + _pos_) % _mrb_->max_len] : (log_w_rtc_line("invalid pos returning 0"), 0)))

// 环形队列的循环遍历,(注意index和rear变量作用域)
#define mcu_ring_buf_forech(_mrb_, _index_) \
    for (uint16_t _index_ = _mrb_->front; _index_ != _mrb_->rear; _index_ = (_index_ + 1) % _mrb_->max_len)

#ifdef __cplusplus
}
#endif

#endif // mcu_ring_buf
