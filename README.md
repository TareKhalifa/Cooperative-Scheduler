# Cooperative-Scheduler
## Scheduler:
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
