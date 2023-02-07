# **Smart Lock (Arduino)**

## **Summary**
- [Description](#description)
- [Design](#design)
- [Code](#code)

## **Description**

This is my project created when I learn IoT from FPT University. I use C language for my project.

My smart lock allows you enter password 3 times, if you enter wrong password 3 times, it will lock down and you must wait 1 minute for the next enter password. Beside that, you can manage at most 4 passwords: 3 user passwords, 1 admin password. You also have full permission to create, update, delete password and change admin password. I will update to give you view all password managed.

Btw, having fun and enjoy it.

## **Design**

![arduino](/images/arduino.png)

## **Code**

Please clone my project, it should be easiest way for you. All code for Main Arduino is in `main.c` and Slave is the same.

```bash
git clone https://github.com/TwentySick/smartlock.git
```
