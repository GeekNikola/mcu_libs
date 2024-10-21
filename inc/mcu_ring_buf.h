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
    //ring_buf ������void* data ������Ϊ��Ҫʹ��������[]��ȡֵ�Ļ���ָ����Ҫǿ��ת����Ӧ���Ͳ�����ȷʹ��
    //��ᵼ�����в������ǳ���data�ĵط���Ҫ��Ҫvoid*��Ӧtype*
    //�������data���������ݣ�����data�Ĳ�������ÿ�ζ������������ֲ���������ǿ��ת�����ʽ�����ȡֵ��
//�����һ��ǿת����������data��ringbuf����,�����궨������ڴ������ⶼ����Ҫ��type������ת�������ת�����ĵ�ʱ��

//�� list ��node�ڵ��޷���ȫ����data ��������Ҫvoid*,�����һЩ���⣬����ͬʱҲ��������Ϊnode��node֮�����Ͷ����Բ�һ��

// Ϊ�˵��ÿⷽ�����ǰ�mcu_ring_buf��api�����궨��ȼ�д��Ϊmrb��Ϊǰ׺
//  �ж϶����Ƿ��:����ͷ�Ͷ�β�غ������Ϊ��
#define mcu_ring_buf_is_empty(_mrb_) (_mrb_->front == _mrb_->rear) // f-r
// �ж϶����Ƿ���:����β����һ���Ƕ�ͷ,�����Ϊ��.ȡ������Ϊ���ó������±�ص����0�����¼���,Ϊ�˺Ͷ���Ϊ�����ֿ�����Ҫ������β������һ���ռ䲻ȥʹ��
#define mcu_ring_buf_is_full(_mrb_) ((_mrb_->rear + 1) % (_mrb_->max_len) == _mrb_->front) //((r+1)%m)==f
// ��ȡ���г���:��β���ڶ�ͷʱ����β����ͷ���ǳ��ȣ����Ƕ��ѭ������ܳ��ֶ�βС�ڶ�ͷ����������Ǹ�����������ʵ����һȦ������Ҫ������󳤶Ⱦͱ�֤���Ǹ�������ȡ������û�д�ʱ�ͽ�ȥ�˲���Ҫ���ϵ���󳤶�
#define mcu_ring_buf_len(_mrb_) ((_mrb_->rear - _mrb_->front + _mrb_->max_len) % _mrb_->max_len) //(r-f+m)%m
// ������0�ص���㣬���ݲ���գ�����һ��ֱ�Ӹ���ԭ����ֵ���ڴ���Ȼռ��
// �������ʹ����malloc���Ƕ���ֻ��Ҫclear�Ϳ�����
#define mcu_ring_buf_clear(_mrb_) (_mrb_->rear = _mrb_->front = 0) // r=f=0

// ���ζ��ж�̬����ģ��
#define mcu_ring_buf_del(_mrb_, _data_) \
    do                                  \
    {                                   \
        mcu_free(_data_);               \
        _data_ = NULL;                  \
        mcu_free(_mrb_);                \
        _mrb_ = NULL;                   \
    } while (0)

// ���Ͷ��ж�̬����ģ��
#define mcu_ring_buf_new(_type_, _mrb_, _data_, _len_)                                                                                                    \
    do                                                                                                                                                    \
    {                                                                                                                                                     \
        _mrb_ = (MCU_OBJ_TYPE(ring_buf) *)mcu_malloc(sizeof(MCU_OBJ_TYPE(ring_buf)));                                                                     \
        if (NULL != _mrb_)                                                                                                                                \
        {                                                                                                                                                 \
            _mrb_->max_len = _len_ + 1; /*���ζ���Ϊ���������кͿն������ֿ���Ҫ����һ����λ��������������max_len=len+1*/ \
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

// ���ζ������ģ��(û�������������)
#define mcu_ring_buf_enqueue(_mrb_, _data_, _val_)                                \
    do                                                                            \
    {                                                                             \
        if (!mcu_ring_buf_is_full(_mrb_))                                         \
        {                                                                         \
            data[_mrb_->rear] = _val_;                        /*�����ݸ�ֵ*/ \
            _mrb_->rear = (_mrb_->rear + 1) % _mrb_->max_len; /*��β�ƶ�*/    \
        }                                                                         \
        else                                                                      \
        {                                                                         \
            log_w_rtc_line("is full cannot enqueue");                             \
        }                                                                         \
    } while (0)

// ���ζ���ȡ��ͷģ��(��Ϊ���򷵻ض�ͷ���ݷ��򷵻�0)
#define mcu_ring_buf_front(_mrb_, _data_) \
  ((!mcu_ring_buf_is_empty(_mrb_)) ? _data_[_mrb_->front] : (log_w_rtc_line("is empty return 0"), 0))

// ���ζ���ȡ��βģ��(��Ϊ���򷵻ض�β���ݷ��򷵻�0)
#define mcu_ring_buf_rear(_mrb_, _data_) \
    ((!mcu_ring_buf_is_empty(_mrb_)) ? _data_[(_mrb_->rear - 1 + _mrb_->max_len) % _mrb_->max_len] : (log_w_rtc_line("is empty return 0"), 0))

// ���ζ��г���ģ��(��Ϊ�����������ӣ�����Ϊ���򷵻�����ֵ���������)
#define mcu_ring_buf_dequeue(_mrb_) \
    _mrb_->front = (!mcu_ring_buf_is_empty(_mrb_)) ? ((_mrb_->front + 1) % _mrb_->max_len) : (log_w_rtc_line("is empty return 0"), _mrb_->front)

// ���ζ��в�ѯֵģ��(�鿴�Ӷ�ͷ��ʼƫ�ƶ���λ�����ݣ�������)
#define mcu_ring_buf_peek(_mrb_, _data_, _pos_) \
    (((_pos_ >= 0 && _pos_ < mcu_ring_buf_len(_mrb_)) ? data[(_mrb_->front + _pos_) % _mrb_->max_len] : (log_w_rtc_line("invalid pos returning 0"), 0)))

// ���ζ��е�ѭ������,(ע��index��rear����������)
#define mcu_ring_buf_forech(_mrb_, _index_) \
    for (uint16_t _index_ = _mrb_->front; _index_ != _mrb_->rear; _index_ = (_index_ + 1) % _mrb_->max_len)

#ifdef __cplusplus
}
#endif

#endif // mcu_ring_buf
