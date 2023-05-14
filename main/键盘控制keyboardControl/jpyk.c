#include <stdio.h>
#include <termios.h>
#include <unistd.h>

#include <stdlib.h>

#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <time.h>
#include <arpa/inet.h>

#include <wiringPi.h>
#include <softPwm.h>

// 定义键盘输入的控制码
#define KEYCODE_W 0x77 // 'w'
#define KEYCODE_S 0x73 // 's'
#define KEYCODE_A 0x61 // 'a'
#define KEYCODE_D 0x64 // 'd'

#define BUFSIZE 512

int PWMA = 1;
int AIN2 = 2;
int AIN1 = 3;

int PWMB = 4;
int BIN2 = 5;
int BIN1 = 6;

// 定义标志变量，表示是否需要停止电机转动
int stop_motor = 1;

void  t_up(unsigned int speed,unsigned int t_time)
{
    stop_motor = 0;
	 digitalWrite(AIN2,0);
	 digitalWrite(AIN1,1);
	 softPwmWrite(PWMA,speed);
	 
	 digitalWrite(BIN2,0);
	 digitalWrite(BIN1,1);
	 softPwmWrite(PWMB,speed);
	 delay(t_time);
}

void t_stop(unsigned int t_time)
{
    stop_motor = 1;
	 digitalWrite(AIN2,0);
	 digitalWrite(AIN1,0);
	 softPwmWrite(PWMA,0);
	 
	 digitalWrite(BIN2,0);
	 digitalWrite(BIN1,0);
	 softPwmWrite(PWMB,0);
	 delay(t_time);	
}

void t_down(unsigned int speed,unsigned int t_time)
{
    stop_motor = 0;
	 digitalWrite(AIN2,1);
	 digitalWrite(AIN1,0);
	 softPwmWrite(PWMA,speed);
	 
	 digitalWrite(BIN2,1);
	 digitalWrite(BIN1,0);
	 softPwmWrite(PWMB,speed);
	 delay(t_time);	
}

void t_left(unsigned int speed,unsigned int t_time)
{
    stop_motor = 0;
	 digitalWrite(AIN2,1);
	 digitalWrite(AIN1,0);
	 softPwmWrite(PWMA,speed);
	 
	 digitalWrite(BIN2,0);
	 digitalWrite(BIN1,1);
	 softPwmWrite(PWMB,speed);
	 delay(t_time);	
}

void t_right(unsigned int speed,unsigned int t_time)
{
    stop_motor = 0;
	 digitalWrite(AIN2,0);
	 digitalWrite(AIN1,1);
	 softPwmWrite(PWMA,speed);
	 
	 digitalWrite(BIN2,1);
	 digitalWrite(BIN1,0);
	 softPwmWrite(PWMB,speed);
	 delay(t_time);	
}






// 获取键盘输入
int getch() {
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

// 主函数
int main() {
      /*RPI*/
    wiringPiSetup();
    /*WiringPi GPIO*/
    pinMode (1, OUTPUT);	//PWMA
    pinMode (2, OUTPUT);	//AIN2
    pinMode (3, OUTPUT);	//AIN1
	
    pinMode (4, OUTPUT);	//PWMB
    pinMode (5, OUTPUT);	//BIN2
	pinMode (6, OUTPUT);    //BIN1
	
	/*PWM output*/
    softPwmCreate(PWMA,0,100);//
	softPwmCreate(PWMB,0,100);  
    

  
    int ch;
    while (1) {
        ch = getch();
        switch (ch) {
            case KEYCODE_W:
                t_up(30,300);
                break;
            case KEYCODE_S:
                t_down(30,300);
                break;
            case KEYCODE_A:
                t_left(30,300);
                break;
            case KEYCODE_D:
                t_right(30,300);
                break;
            default:
                t_stop(300);
                break;
        }
        
        //sleep(1); 控制电机转动一秒钟
        if (stop_motor) {
            // 停止电机转动
            t_stop(300);
        }
    }
    return 0;
}