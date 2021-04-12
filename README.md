# Cooperative-Scheduler
## Scheduler:
#### Implementation:
##### 1. void init(void):
This is the initialization function of the scheduler. It initializes the queue and the delayed queue capacity, current size, and tasks.
##### 2. void swapTasks(taskStruct *one, taskStruct *two):
This function is used to arrange tasks in the queue based on their priority. It takes two tasks and swaps their positions in the queue.
##### 3. void insertt(queueStruct *q, taskStruct t):
This function is the one responsible for directly inserting tasks into the queues. It takes the queue pointer and the task to be inserted into the queue. It assigns the task a position in the queue and creates a task, then assigns the calling task to it. It then inserts this task into the queue. After inserting it into the queue, the function calls swapTasks function to swap the task if needed to maintain the priority in the queue.
##### 4. void enqueue(fnPtr fPointer, int priority, int delay):
This function is used to take tasks to be isnerted into the queue. It takes the task's pointer, priority and delay. Then, it calls insertt(queueStruct *q, taskStruct t) to actually insert the function to the queue.
##### 5. void rerunMe(queueStruct *q, taskStruct t):
This function is the one that a task uses to insert itself into the ready or the delayed queues. To insert itself into the queue, the function calls swapTasks function to take its right place in the queue based on its priority. The task chooses which queue it will be inserted in by sending a pointer to that queue when calling the function and a pointer to the task itself.
##### 6. taskStruct *dequeue(queueStruct *q):

##### 7. void func(void):
This is a test function used as a dummy task to test the schedule.

## Applications:
### 1. Ambient Temperature Monitor:
![image](https://user-images.githubusercontent.com/59120814/114321947-e4357e80-9b1d-11eb-9162-3b63274e43de.png)

In this application, DS3231 RTC module is used to measure the Ambient Temperature. The sensor is set to detect the temperature every 30 seconds. The system receives a threshold value n through an asynchronous serial link (UART1). It compares this value to the detected Ambient Temperature and if it reaches that threshold, a led starts flashing as a form of alarm.
#### Implementation:
##### 1. ReadTemp():
This function is the one responsible for reading the temperature. First, the temprature registers of the DS3231 RTC module are set and read over the I2C bus. Then, using the systick to detect the 30 seconds passing, the control and status registers are used to force the temperature sensor to read the ambient temperature value every 30 seconds instead of the default 64 seconds. 
After reading the temperature value, the function compares this value to the threshold sent by the PC over asynchronous serial link UART1. If the ambient temperature exceeds the threshold, a flag [on] is set to 1.
##### 2. TriggerAlarm():
This function is the one responsibe for flashing the LED after the temperature passes the threshold.
When the on flag is set to 1, it flashes the LED. 
##### 3. USART1_IRQHandler():
In the USART1 handler, the threshold value is received from the PC.
