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
    uint16_t len;
    void *msg;
    MCU_OBJ_END(list)

// 动态内存分配
#define mcu_list_node_new (MCU_OBJ_TYPE(node) *)mcu_malloc(sizeof(MCU_OBJ_TYPE(node))) // 新建单链表结点，动态分配内存
#define mcu_list_node_del (node) mcu_free(node);                                       // 删除链表结点
#define mcu_list_is_empty(ml) (ml)->len == 0                                           // 判断链表是否为空
#define mcu_list_forech(ml, index) \
    for (index = ml->head, uint16_t cnt = 0; cnt < ml->len; index = index->next, cnt++)

#define mcu_list_new(ml, msg)                                          \
    do                                                                 \
    {                                                                  \
        ml = (MCU_OBJ_TYPE(list) *)malloc(sizeof(MCU_OBJ_TYPE(list))); \
        if (NULL != ml)                                                \
        {                                                              \
            ml->head = NULL;                                           \
            ml->tail = NULL;                                           \
            ml->len = 0;                                               \
            ml->msg = msg;                                             \
        }                                                              \
    } while (0)

#define mcu_list_get_node(ml, node, pos)                  \
    do                                                    \
    {                                                     \
        if (NULL != ml && NULL != ml->head pos < ml->len) \
        {                                                 \
            if ((pos) == ml->len - 1)                     \
            {                                             \
                node = ml->tail;                          \
            }                                             \
            else                                          \
            {                                             \
                mcu_list_forech(ml, node_i)               \
                {                                         \
                    if (cnt == (pos))                     \
                    {                                     \
                        node = node_i;                    \
                    }                                     \
                }                                         \
            }                                             \
        }                                                 \
    } while (0)

#define mcu_list_insert(ml, type, data, size, pos)                           \
    do                                                                       \
    {                                                                        \
        MCU_OBJ_TYPE(node) *node_prev = NULL;                                \
        MCU_OBJ_TYPE(node) *new_node = (MCU_OBJ_TYPE(list) *)(malloc(size)); \
        if (NULL != ml && NULL != new_node)                                  \
        {                                                                    \
            new_node->data = data;                                           \
            new_node->data_size = data_size;                                 \
            new_node->next = NULL;                                           \
            if (mcu_list_is_empty(ml))                                       \
            {                                                                \
                ml->head = new_node;                                         \
                ml->tail = new_node;                                         \
                new_node->next = ml->head;                                   \
            }                                                                \
            else                                                             \
            {                                                                \
                if (NULL != ml->head)                                        \
                {                                                            \
                    if (pos == 0 && ml->head != new_node)                    \
                    {                                                        \
                        new_node->next = ml->head;                           \
                        ml->head = new_node;                                 \
                        ml->tail->next = ml->head;                           \
                        ml->len++;                                           \
                    }                                                        \
                    else                                                     \
                    {                                                        \
                        node_prev = mcu_list_get_node(ml, pos - 1);          \
                        if (node_prev && (node_prev != new_node))            \
                        {                                                    \
                            new_node->next = node_prev->next;                \
                            node_prev->next = new_node;                      \
                            if (pos == ml->len)                              \
                            {                                                \
                                ml->tail = new_node;                         \
                            }                                                \
                            ml->len++;                                       \
                        }                                                    \
                    }                                                        \
                }                                                            \
            }                                                                \
        }                                                                    \
    } while (0)

#define mcu_list_push_front(ml, type, data, size) mcu_list_insert(ml, type, data, size, 0)
#define mcu_list_push_back(ml, type, data, size) mcu_list_insert(ml, type, data, size, ml->len)

#define mcu_list_erase(ml, pos)                                         \
    do                                                                  \
    {                                                                   \
        sll_node_t *node_prev = NULL;                                   \
        sll_node_t *del_node = NULL;                                    \
        if (NULL != ml && (!mcu_list_is_empty(ml)) && NULL != ml->head) \
        {                                                               \
            if (pos == 0)                                               \
            {                                                           \
                del_node = ml->head;                                    \
                ml->head = del_node->next;                              \
                ml->tail->next = ml->head;                              \
                mcu_list_node_del(del_node);                            \
                ml->len--;                                              \
            }                                                           \
            else                                                        \
            {                                                           \
                node_prev = mcu_list_get_node(sll, pos - 1);            \
                if (node_prev)                                          \
                {                                                       \
                    del_node = node_prev->next;                         \
                    node_prev->next = del_node->next;                   \
                    mcu_list_node_del(del_node);                        \
                    if (pos == sll->len - 1)                            \
                    {                                                   \
                        sll->tail = node_prev;                          \
                    }                                                   \
                    sll->len--;                                         \
                }                                                       \
            }                                                           \
        }                                                               \
    } while (0)

#define mcu_list_erase_range(ml, sta, end)     \
    do                                         \
    {                                          \
        uint32_t cnt = 0;                      \
        for (cnt = 0; cnt <= end - sta; cnt++) \
            mcu_list_erase(sll, sta);          \
    } while (0)

#define mcu_list_pop_front(ml) mcu_list_erase(ml, 0)
#define mcu_list_pop_back(ml) mcu_list_erase(ml, ml->len - 1)

#define mcu_list_clear(ml)     \
    while (!is_sll_empty(ml))  \
    {                          \
        mcu_list_erase(ml, 0); \
    }

#ifdef __cplusplus
}
#endif

#endif // MCU_LIST_H
