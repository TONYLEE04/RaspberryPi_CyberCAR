#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <time.h>
#include <arpa/inet.h>

#include <wiringPi.h>
#include <softPwm.h>

#define BUFSIZE 512

int PWMA = 1;
int AIN2 = 2;
int AIN1 = 3;

int PWMB = 4;
int BIN2 = 5;
int BIN1 = 6;

#define LEFT	23
#define RIGHT	25

void  t_up(unsigned int speed,unsigned int t_time)
{
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
	 digitalWrite(AIN2,0);
	 digitalWrite(AIN1,1);
	 softPwmWrite(PWMA,speed);
	 
	 digitalWrite(BIN2,1);
	 digitalWrite(BIN1,0);
	 softPwmWrite(PWMB,speed);
	 delay(t_time);	
}

int main(int argc, char *argv[])
{
    float dis;
   // char buf[BUFSIZE]={0xff,0x00,0x00,0x00,0xff};
	int SR;
	int SL;
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

 while(1)
  {
  //有信号为LOW  没有信号为HIGH
  SR = digitalRead(RIGHT);//有信号表明在白色区域，车子底板上L亮；没信号表明压在黑线上，车子底板上L灭
  SL = digitalRead(LEFT);//有信号表明在白色区域，车子底板上L亮；没信号表明压在黑线上，车子底板上L灭
  if (SL == LOW&&SR==LOW){
   printf("GO");
   t_up(50,0);
   
  }
  else if (SL == HIGH&&SR == LOW){
	  printf("LEFT");

	 t_left(50,0);
	  
  }
  else if (SR == HIGH&&SL == LOW) {// 右循迹红外传感器,检测到信号，车子向左偏离轨道，向右转  
      printf("RIGHT");  
      t_right(50,0);
  }
  else {// 都是白色, 停止
     printf("STOP");
     t_stop(0);
    }
  }
  return 0;

}

