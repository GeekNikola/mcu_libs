#if !defined(MCU_LIST_H)
#define MCU_LIST_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "mcu_config.h"
#include "mcu_loger.h"

    // 单向循环链表节点结构
    MCU_OBJ_START(node)
    void *data;
    struct obj_node *next;
    MCU_OBJ_END(node);

    // 单向循环链表结构
    MCU_OBJ_START(list)
    MCU_OBJ_TYPE(node) * head;
    MCU_OBJ_TYPE(node) * tail; // 尾结点
    int16_t pos;
    int16_t len;
    void *msg;
    MCU_OBJ_END(list)

// 动态内存分配
#define mcu_list_node_new (MCU_OBJ_TYPE(node) *)mcu_malloc(sizeof(MCU_OBJ_TYPE(node))) // 新建单链表结点，动态分配内存
#define mcu_list_is_empty(_ml_) ((_ml_)->len == 0)                                       // 判断链表是否为空
#define mcu_list_forech(_ml_, _index_) \
    _ml_->pos = 0;                     \
    for (MCU_OBJ_TYPE(node) *_index_ = _ml_->head; _ml_->pos < _ml_->len; _index_ = _index_->next, _ml_->pos++)

#define mcu_list_new(_ml_, _msg_)                                            \
    do                                                                       \
    {                                                                        \
        _ml_ = (MCU_OBJ_TYPE(list) *)mcu_malloc(sizeof(MCU_OBJ_TYPE(list))); \
        if (NULL != _ml_)                                                    \
        {                                                                    \
            _ml_->head = NULL;                                               \
            _ml_->tail = NULL;                                               \
            _ml_->len = 0;                                                   \
            _ml_->msg = _msg_;                                               \
            break;                                                           \
        }                                                                    \
        log_e_rtc_line("list new fail");                                     \
    } while (0)

#define mcu_list_get_node(_ml_, _node_, _pos_)                     \
    do                                                             \
    {                                                              \
        if (NULL != _ml_ && NULL != _ml_->head && _pos_ < _ml_->len) \
        {                                                          \
            if ((_pos_) == _ml_->len - 1)                          \
            {                                                      \
                _node_ = _ml_->tail;                               \
            }                                                      \
            else                                                   \
            {                                                      \
                mcu_list_forech(_ml_, node_i)                      \
                {                                                  \
                    if (_ml_->pos == (_pos_))                        \
                    {                                              \
                        _node_ = node_i;                           \
                    }                                              \
                }                                                  \
            }                                                      \
        }                                                          \
    } while (0)

#define mcu_list_insert(_ml_, _data_addr_, _size_, _pos_)                  \
    do                                                                             \
    {                                                                              \
        MCU_OBJ_TYPE(node) *node_prev = NULL;                                      \
        MCU_OBJ_TYPE(node) *new_node = (MCU_OBJ_TYPE(node) *)(mcu_malloc(_size_)); \
        if (NULL != _ml_ && NULL != new_node)                                      \
        {                                                                          \
            new_node->data = _data_addr_;                                          \
            new_node->next = NULL;                                                 \
            if (mcu_list_is_empty(_ml_))                                           \
            {                                                                      \
                _ml_->head = new_node;                                             \
                _ml_->tail = new_node;                                             \
                new_node->next = _ml_->head;                                       \
                _ml_->len++;                                                       \
            }                                                                      \
            else                                                                   \
            {                                                                      \
                if (NULL != _ml_->head)                                            \
                {                                                                  \
                    if (_pos_ == 0 && _ml_->head != new_node)                      \
                    {                                                              \
                        new_node->next = _ml_->head;                               \
                        _ml_->head = new_node;                                     \
                        _ml_->tail->next = _ml_->head;                             \
                        _ml_->len++;                                               \
                    }                                                              \
                    else                                                           \
                    {                                                              \
                        mcu_list_get_node(_ml_, node_prev, _pos_ - 1);        \
                        if (node_prev && (node_prev != new_node))                  \
                        {                                                          \
                            new_node->next = node_prev->next;                      \
                            node_prev->next = new_node;                            \
                            if (_pos_ == _ml_->len)                           \
                            {                                                      \
                                _ml_->tail = new_node;                             \
                            }                                                      \
                            _ml_->len++;                                           \
                        }                                                          \
                    }                                                              \
                }                                                                  \
            }                                                                      \
        }                                                                          \
    } while (0)

#define mcu_list_push_front(_ml_,  _data_addr_, _size_) mcu_list_insert(_ml_, _data_addr_, _size_, 0)
#define mcu_list_push_back(_ml_,  _data_addr_, _size_) mcu_list_insert(_ml_, _data_addr_, _size_, _ml_->len)

#define mcu_list_erase(_ml_, _pos_)                                           \
    do                                                                        \
    {                                                                         \
        MCU_OBJ_TYPE(node) *node_prev = NULL;                                 \
        MCU_OBJ_TYPE(node) *del_node = NULL;                                  \
        if (NULL != _ml_ && (!mcu_list_is_empty(_ml_)) && NULL != _ml_->head) \
        {                                                                     \
            if (_pos_ == 0)                                                   \
            {                                                                 \
                del_node = _ml_->head;                                        \
                _ml_->head = del_node->next;                                  \
                _ml_->tail->next = _ml_->head;                                \
                mcu_free(del_node->data);                                     \
                mcu_free(del_node);                                           \
                _ml_->len--;                                                  \
            }                                                                 \
            else                                                              \
            {                                                                 \
                mcu_list_get_node(_ml_, node_prev, _pos_ - 1);                \
                if (node_prev)                                                \
                {                                                             \
                    del_node = node_prev->next;                               \
                    node_prev->next = del_node->next;                         \
                    mcu_free(del_node->data);                                     \
                    mcu_free(del_node);                                       \
                    if (_pos_ == _ml_->len - 1)                                 \
                    {                                                         \
                        _ml_->tail = node_prev;                               \
                    }                                                         \
                    _ml_->len--;                                              \
                }                                                             \
            }                                                                 \
        }                                                                     \
    } while (0)

#define mcu_list_erase_range(_ml_, _sta_, _end_)   \
    do                                             \
    {                                              \
        uint32_t cnt = 0;                          \
        for (cnt = 0; cnt <= _end_ - _sta_; cnt++) \
            mcu_list_erase(_ml_, _sta_);             \
    } while (0)

#define mcu_list_pop_front(_ml_) mcu_list_erase(_ml_, 0)
#define mcu_list_pop_back(_ml_) mcu_list_erase(_ml_, _ml_->len - 1)

#define mcu_list_clear(_ml_)         \
    while (!mcu_list_is_empty(_ml_)) \
    {                                \
        mcu_list_erase(_ml_, 0);     \
    }

#ifdef __cplusplus
}
#endif

#endif // MCU_LIST_H
