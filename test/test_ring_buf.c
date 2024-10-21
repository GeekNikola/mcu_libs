#include "mcu_libs.h"


// ����ṹ������
typedef struct {
    int id;
    char name[20];
} DataStruct;

// ��ӡ���λ������е�����
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

// ���Ի��λ���������
void test_ring_buf(void) {
    MCU_OBJ_TYPE(ring_buf) *rbuf;
    DataStruct *data;

    // �������λ�����
    mcu_ring_buf_new(DataStruct, rbuf, data, 5);

    // ��������
    DataStruct ds1 = {1, "Alice"};
    DataStruct ds2 = {2, "Bob"};
    DataStruct ds3 = {3, "Charlie"};

    // ��Ӳ���
    mcu_ring_buf_enqueue(rbuf, data, ds1);
    mcu_ring_buf_enqueue(rbuf, data, ds2);
    mcu_ring_buf_enqueue(rbuf, data, ds3);

    // ��ӡ���λ�����״̬
    print_ring_buf(rbuf, data);

    // ���Ӳ���
    mcu_ring_buf_dequeue(rbuf);
    printf("After dequeue:\n");
    print_ring_buf(rbuf, data);

    // ��ղ�ɾ�����λ�����
    mcu_ring_buf_clear(rbuf);
    mcu_ring_buf_del(rbuf, data);
	print_ring_buf(rbuf, data);
}