#include "sched.h"
queueStruct qu;
queueStruct delayed;
taskStruct aa[100];
taskStruct bb[100];
void rerunMe(queueStruct *q, taskStruct t)
{
    volatile int pos = q->current_sz++;

    fptr x = t.fPointer;
    volatile struct task t1 = t;
    q->qTasks[pos] = t1;
    if (pos == 0)
        return;
    while ((pos != 0) && (t1.delay < q->qTasks[--pos].delay))
        swapTasks(&q->qTasks[pos + 1], &q->qTasks[pos]);
    return;
}
void swapTasks(taskStruct *one, taskStruct *two)
{
    volatile taskStruct temp = *one;
    *one = *two;
    *two = temp;
}
void init(void)
{
    qu.capacity = 100;
    qu.current_sz = 0;
    qu.qTasks = aa;
    delayed.capacity = 100;
    delayed.current_sz = 0;
    delayed.qTasks = bb;
}
void enqueue(queueStruct *q, taskStruct t)
{
    volatile int pos = q->current_sz++;

    fptr x = t.fPointer;
    volatile struct task t1 = t;
    q->qTasks[pos] = t1;
    if (pos == 0)
        return;
    while ((pos != 0) && (t1.priority < q->qTasks[--pos].priority))
        swapTasks(&q->qTasks[pos + 1], &q->qTasks[pos]);
    return;
}
taskStruct *dequeue(queueStruct *q)
{
    for (int i = 0; i < q->current_sz - 1; i++)
        swapTasks(&q->qTasks[i], &q->qTasks[i + 1]);
    taskStruct *temp = &q->qTasks[q->current_sz - 1];
    q->current_sz--;
    return temp;
}