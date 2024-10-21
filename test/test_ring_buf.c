#include "mcu_libs.h"


// 定义结构体数据
typedef struct {
    int id;
    char name[20];
} DataStruct;

// 打印环形缓冲区中的数据
void print_ring_buf(MCU_OBJ_TYPE(ring_buf) *rbuf, DataStruct *data) {
    if (mcu_ring_buf_is_empty(rbuf)) {
        printf("Ring buffer is empty.\n");
        return;
    }

    printf("Ring buffer elements:\n");
    mcu_ring_buf_forech(rbuf, index) {
        printf("ID: %d, Name: %s\n", data[index].id, data[index].name);
    }
}

// 测试环形缓冲区操作
void test_ring_buf(void) {
    MCU_OBJ_TYPE(ring_buf) *rbuf;
    DataStruct *data;

    // 创建环形缓冲区
    mcu_ring_buf_new(DataStruct, rbuf, data, 5);

    // 定义数据
    DataStruct ds1 = {1, "Alice"};
    DataStruct ds2 = {2, "Bob"};
    DataStruct ds3 = {3, "Charlie"};

    // 入队操作
    mcu_ring_buf_enqueue(rbuf, data, ds1);
    mcu_ring_buf_enqueue(rbuf, data, ds2);
    mcu_ring_buf_enqueue(rbuf, data, ds3);

    // 打印环形缓冲区状态
    print_ring_buf(rbuf, data);

    // 出队操作
    mcu_ring_buf_dequeue(rbuf);
    printf("After dequeue:\n");
    print_ring_buf(rbuf, data);

    // 清空并删除环形缓冲区
    mcu_ring_buf_clear(rbuf);
    mcu_ring_buf_del(rbuf, data);
	print_ring_buf(rbuf, data);
}