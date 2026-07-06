/*
 * debug_practice.c — GDB 调试 & 日志调试综合练习
 *
 * 功能：简易成绩管理系统（动态数组）
 * 练习点：printf 日志 / assert 断言 / DEBUG 宏 / GDB 调试
 *
 * 编译：gcc -Wall -g -DDEBUG debug_practice.c -o dbg
 *       gcc -Wall -g debug_practice.c -o dbg       （关闭 DEBUG 输出）
 * 调试：gdb ./dbg
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

/* ———— 调试宏 ———— */
#define DEBUG              /* 注释掉本行即可关闭调试输出 */

#ifdef DEBUG
    #define debug(x) printf("[DEBUG] %s:%d  %s = %d\n", __FUNCTION__, __LINE__, #x, x)
    #define debug_s(x) printf("[DEBUG] %s:%d  %s = %s\n", __FUNCTION__, __LINE__, #x, x)
#else
    #define debug(x)
    #define debug_s(x)
#endif

#define Debug() printf(">>> BUG IN %s (file: %s), @line: %d, compiled %s %s <<<\n", \
                       __FUNCTION__, __FILE__, __LINE__, __TIME__, __DATE__)

/* ———— 动态数组结构 ———— */
typedef struct {
    int *data;      /* 数据指针 */
    int size;       /* 当前元素个数 */
    int capacity;   /* 最大容量 */
} ScoreList;

/* 初始化 */
void initList(ScoreList *list, int cap) {
    list->data = (int *)malloc(sizeof(int) * cap);
    assert(list->data != NULL);          /* 断言：内存分配不能失败 */
    list->size = 0;
    list->capacity = cap;
    printf("  [init] 列表已创建，容量=%d\n", cap);
}

/* 扩容（内部函数） */
static void expand(ScoreList *list) {
    int new_cap = list->capacity * 2;
    debug(list->capacity);
    debug(new_cap);
    list->data = (int *)realloc(list->data, sizeof(int) * new_cap);
    assert(list->data != NULL);
    list->capacity = new_cap;
    printf("  [expand] 扩容至 %d\n", new_cap);
}

/* 添加成绩 */
void addScore(ScoreList *list, int score) {
    assert(score >= 0 && score <= 100);  /* 断言：成绩合法 */
    if (list->size >= list->capacity) {
        expand(list);
    }
    list->data[list->size] = score;
    list->size++;
    debug(score);
    debug(list->size);
    printf("  [add] 添加成绩: %d (当前共 %d 条)\n", score, list->size);
}

/* 删除末尾 */
int popScore(ScoreList *list) {
    assert(list->size > 0);              /* 断言：不能对空列表 pop */
    list->size--;
    int val = list->data[list->size];
    debug(val);
    printf("  [pop] 弹出成绩: %d (剩余 %d 条)\n", val, list->size);
    return val;
}

/* 按索引删除 */
void removeAt(ScoreList *list, int index) {
    assert(index >= 0 && index < list->size);
    debug(index);
    printf("  [remove] 删除第 %d 个成绩: %d\n", index, list->data[index]);

    /* 把后面的元素往前移 */
    for (int i = index; i < list->size - 1; i++) {
        list->data[i] = list->data[i + 1];
    }
    list->size--;
}

/* 计算平均分 */
double calcAverage(ScoreList *list) {
    if (list->size == 0) {
        Debug();                         /* 日志：对空列表求平均 */
        return 0.0;
    }
    int sum = 0;
    for (int i = 0; i < list->size; i++) {
        sum += list->data[i];
    }
    debug(sum);
    debug(list->size);
    return (double)sum / list->size;
}

/* 查找最高分 */
int findMax(ScoreList *list) {
    assert(list->size > 0);
    int max_val = list->data[0];
    int max_idx = 0;
    for (int i = 1; i < list->size; i++) {
        if (list->data[i] > max_val) {
            max_val = list->data[i];
            max_idx = i;
        }
    }
    debug(max_val);
    debug(max_idx);
    printf("  [max] 最高分: %d (索引 %d)\n", max_val, max_idx);
    return max_val;
}

/* 打印全部成绩 */
void printList(ScoreList *list) {
    printf("  [list] 成绩单 (共 %d 条): ", list->size);
    for (int i = 0; i < list->size; i++) {
        printf("%d ", list->data[i]);
    }
    printf("\n");
}

/* 释放内存 */
void destroyList(ScoreList *list) {
    free(list->data);
    list->data = NULL;   /* ⚠️ 先注释掉这行，用 GDB 观察 use-after-free */
    list->size = 0;
    list->capacity = 0;
    printf("  [destroy] 内存已释放\n");
}

/* ———— 主程序 ———— */
int main() {
    ScoreList list;
    printf("===== 成绩管理系统 =====\n\n");

    /* 1. 初始化 */
    printf("1. 初始化:\n");
    initList(&list, 2);                  /* 容量只给 2，触发扩容 */

    /* 2. 添加成绩 */
    printf("\n2. 添加成绩:\n");
    addScore(&list, 85);
    addScore(&list, 92);
    addScore(&list, 78);                 /* 这里会触发扩容！ */
    addScore(&list, 95);
    addScore(&list, 60);

    /* 3. 打印列表 */
    printf("\n3. 打印:\n");
    printList(&list);

    /* 4. 统计 */
    printf("\n4. 统计:\n");
    double avg = calcAverage(&list);
    printf("  [avg] 平均分: %.2f\n", avg);
    printf("  [max] 最高分: %d\n", findMax(&list));

    /* 5. 删除 */
    printf("\n5. 删除:\n");
    removeAt(&list, 2);                  /* 删除索引 2（78 分） */
    printList(&list);

    popScore(&list);                     /* 弹出末尾（60 分） */
    printList(&list);

    /* 6. 释放 */
    printf("\n6. 释放:\n");
    destroyList(&list);

    /* ——— 以下注释掉的代码用于练习 GDB ——— */
    /* 练习 1：取消注释下面这行，用 GDB 观察 use-after-free
    printf("  [bug] 释放后访问: %d\n", list.data[0]);  */
 


    /* 练习 2：取消注释下面这行，用 GDB 观察 assert 失败
    removeAt(&list, 999);  */

    /* 练习 3：取消注释下面这行，用 GDB 观察段错误
    free(list.data); */

    printf("\n===== 程序正常结束 =====\n");
    return 0;
}
