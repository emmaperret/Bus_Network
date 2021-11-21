# Labs of Bus and Network
#### ENSEA 2022 Lab team-work Emma PERRET and Nicolas GUERIN
This lab aims at giving us the ability to create a chain of communication between a sensor, microcontroler, and an internet interface. The following communication types will be established:
1. The control of the sensor will be done thanks to **I2C** protocol.
2. The communication between the motor and the STM32 microcontroller will be achieved thanks to the CAN (**Controller Area Network**) protocol.
3. The communication between the Raspberry Pi and the STM32 controller will be done thanks to UART communication.
4. The wifi protocol will be used to communicate between the server on the raspberry pi and the STM32 microcontroller.

## Communicating with the STM32F476
### USART2 to enable programming and debug thanks to printf redirection
To enable a proper understanding and the use of the shell command tool. You need to activate the communication on **USART 2** thanks to the pin connected to the USB ST-Link tool: **PA2** et **PA3**. We can have access to shell thanks to minicom on Linux or PuTTY on Windows. We have enable redirection of the `printf` weak command to allow us to use on the microcontroller.

### Setting the sensor communication : I2C protocol
The I2C protocol is a serial protocol which takes and address to enable identification of the **slave**. To get the data from the sensor thanks to I2C, in the shell enter :
> <T> 'now','all' to see wheter the last or all the temperature you have requested since the start.
Same goes for pressure with the following command:
> <P> 'now','all'
The perfect move would have been to directly create rotation of the motor according to the received temperature multiplied by a coefficient that would have enabled us to follow the real time evolution of temperature. We were not very far from that peculiar point, we can collect the temperature as hexadecimal data.

### Communication with the Raspberry Pi
For this communication we have activated the UART4 port. We have therefore modified the printf function so that the display is done on both USART ports.
This project is made in **another** branch be careful to download both to test this code. Indeed, we had not the time to implement this part of our work in the shell command base. Therefore we had no other option than to upload this part of the program outside our working code. We had trouble testing the communication between the internet and the raspberry pi. Yet the communcation between the Raspberry and the STM32 microcontroller use to work properly before it crashed due to a bad manipulation. 





## Using CAN protocol to initiate movement
The CAN protocol could have allowed us to manipulate serveral mechanical modules. We implemented the command thanks to the PIN  **  PB8 ** and ** PB9 ** on the CAN1 available setup in the microcontroller.
The function `M` in the `shell` is made to allow movement of the motor thanks to CAN communication. You cannot change the angle of rotation, which is set by default to 90 degrees.

## Coding the internet interface 
We followed step by step the beginning of the REST Interface part.
The JSON principle was a bit difficult to understand, as well as the code related to HTTP methods.
We still managed to make the GET and POST methods in the hello.py file.
However, when we had to adapt all this to the creation of the web page of our project, it was immediately more complicated. What blocked us was the link between the STM part with the recovery of temperatures and pressures and the creation of the web page. Moreover, we did not have time to understand how to set up the PUT and DELETE methods.
So we couldn't get the code to work and we couldn't fix the errors in the time available.
