



#include "mcu_libs.h"




// �������ݽṹ
typedef struct {
    int id;
    char name[20];
} DataStruct;

// ��ӡ�������ݵĺ���
void print_list(MCU_OBJ_TYPE(list) *ml) {
    if (mcu_list_is_empty(ml)) {
        printf("List is empty.\n");
        return;
    }
    printf("msg: %s\n", ml->msg);
    //MCU_OBJ_TYPE(node) *node = NULL;
    printf("List elements:\n");
    mcu_list_forech(ml, node) {
        DataStruct *data = (DataStruct *)(node->data);
        printf("ID: %d, Name: %s\n", data->id, data->name);
    }
}

void test_list(void)
{
     // �������������ڴ�
    MCU_OBJ_TYPE(list) *ml = NULL;
    mcu_list_new(ml, "Test List");

    // �������� DataStruct ʵ��
    DataStruct data1 = {1, "Alice"};
    DataStruct data2 = {2, "Bob"};
    DataStruct data3 = {3, "Charlie"};

    // ���� push_front�����뵽����ͷ��
    mcu_list_push_front(ml,  &data1,sizeof(DataStruct));
    mcu_list_push_front(ml,  &data2,sizeof(DataStruct));	
	mcu_list_push_front(ml, &data3,sizeof(DataStruct));
	
    print_list(ml);  // ���: Alice

    // ���� push_back�����뵽����β��
    mcu_list_push_back(ml,  &data2, sizeof(DataStruct));
    print_list(ml);  // ���: Alice, Bob

    // ���� insert�����뵽���� 1 ��λ��
    mcu_list_insert(ml,  &data1, sizeof(DataStruct), 1);  // ������1����
    print_list(ml);  // ���: Alice, Charlie, Bob

    // ���� erase��ɾ������ 1 ��Ԫ��
    mcu_list_erase(ml, 1);  // ɾ������1��Ԫ��
    print_list(ml);  // ���: Alice, Bob

    // ���� erase��ɾ������ 1 ��Ԫ��
    mcu_list_erase(ml, 123);  // ɾ������1��Ԫ��
    print_list(ml);  // ���: Alice, Bob
	
	
    // ���� clear���������
    mcu_list_clear(ml);
    print_list(ml);  // ���: List is empty
}

