



#include "mcu_libs.h"




// 定义数据结构
typedef struct {
    int id;
    char name[20];
} DataStruct;

// 打印链表内容的函数
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
     // 创建链表并分配内存
    MCU_OBJ_TYPE(list) *ml = NULL;
    mcu_list_new(ml, "Test List");

    // 创建几个 DataStruct 实例
    DataStruct data1 = {1, "Alice"};
    DataStruct data2 = {2, "Bob"};
    DataStruct data3 = {3, "Charlie"};

    // 测试 push_front，插入到链表头部
    mcu_list_push_front(ml,  &data1,sizeof(DataStruct));
    mcu_list_push_front(ml,  &data2,sizeof(DataStruct));	
	mcu_list_push_front(ml, &data3,sizeof(DataStruct));
	
    print_list(ml);  // 输出: Alice

    // 测试 push_back，插入到链表尾部
    mcu_list_push_back(ml,  &data2, sizeof(DataStruct));
    print_list(ml);  // 输出: Alice, Bob

    // 测试 insert，插入到索引 1 的位置
    mcu_list_insert(ml,  &data1, sizeof(DataStruct), 1);  // 在索引1插入
    print_list(ml);  // 输出: Alice, Charlie, Bob

    // 测试 erase，删除索引 1 的元素
    mcu_list_erase(ml, 1);  // 删除索引1的元素
    print_list(ml);  // 输出: Alice, Bob

    // 测试 erase，删除索引 1 的元素
    mcu_list_erase(ml, 123);  // 删除索引1的元素
    print_list(ml);  // 输出: Alice, Bob
	
	
    // 测试 clear，清空链表
    mcu_list_clear(ml);
    print_list(ml);  // 输出: List is empty
}

