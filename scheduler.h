
typedef void (*fnPtr)(void);
typedef struct task
{
    fnPtr fPointer;
    int priority;
    int delay;
} taskStruct;

typedef struct tQueue
{
    int capacity;
    int current_sz;
    taskStruct *qTasks;
} queueStruct;
void dispatch(void);
void rerun(queueStruct *q, taskStruct t);
taskStruct *dequeue(queueStruct *q);
void rerunMe(fnPtr fPointer, int priority, int delay);
void swapTasks(taskStruct *one, taskStruct *two);
void init(void);
void enqueue(fnPtr fPointer, int priority, int delay);
void func(void);
void insertt(queueStruct *q, taskStruct t);
queueStruct qu;
queueStruct delayed;
taskStruct aa[100];
taskStruct bb[100];
void rerunMe(fnPtr fPointer, int priority, int delay)
{
    volatile taskStruct t = {fPointer, priority, delay};
    if (delay == 0)
        insertt(&qu, t);
    else
        rerun(&delayed, t);
}
void rerun(queueStruct *q, taskStruct t)
{
    volatile int pos = q->current_sz++;

    volatile struct task t1 = t;
    q->qTasks[pos] = t1;
    if (pos == 0)
        return;
    while ((pos != 0) && (t1.delay < q->qTasks[--pos].delay))
        swapTasks(&q->qTasks[pos + 1], &q->qTasks[pos]);
    return;
}
void dispatch(void)
{
    for (int i = 0; i < delayed.current_sz; i++)
    {
        if (delayed.qTasks[i].delay > 0)
            delayed.qTasks[i].delay--;
    }

    while (delayed.current_sz > 0 && delayed.qTasks[0].delay == 0)
        insertt(&qu, *dequeue(&delayed));
    if (qu.current_sz > 0)
        dequeue(&qu)->fPointer();
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
void enqueue(fnPtr fPointer, int priority, int delay)
{
    volatile taskStruct t = {fPointer, priority, delay};
    insertt(&qu, t);
}
void insertt(queueStruct *q, taskStruct t)
{
    volatile int pos = q->current_sz++;
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
