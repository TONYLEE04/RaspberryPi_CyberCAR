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
//北极蓝线，南极橙线
//左侧接口接左侧传感器，右侧接右侧
#define LEFT_N	28
#define LEFT_S  29
#define RIGHT_N	 26
#define RIGHT_S  27

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
	int L_N;
	int L_S;
	int R_N;
	int R_S;
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
    softPwmCreate(PWMA,0,100);
	softPwmCreate(PWMB,0,100);  

 while(1)
  {
  
  L_N = digitalRead(LEFT_N);
  L_S = digitalRead(LEFT_S);
  R_N = digitalRead(RIGHT_N);
  R_S = digitalRead(RIGHT_S);

  //无讯号输入，慢速直行
  if (L_N == LOW && R_N==LOW && L_S==LOW && R_S==LOW){
   printf("NO SIGNAL");
   t_up(25,0);
  }

  //左右两侧北极输入高电平：出发信号，直行通过大直道
  else if (L_N == HIGH && R_N ==HIGH){
	 printf("START ");

	 t_up(50,1000);
	 sleep(1);
  }

  //左侧北极高电平：左转
  else if (L_N == HIGH&&R_N ==LOW){
	 printf("LEFT");

	 t_left(50,800);
	 sleep(1);
	  
  }

  //右侧北极高电平:	右转
  else if (R_N == HIGH&&L_N ==LOW) {
      printf("RIGHT");  

      t_right(50,800);
	  sleep(1);
  }

  //左侧南极高电平：？？？
  else if (L_S == HIGH && R_S ==LOW){
	 printf("???");

	 t_left(0,0);
	 sleep(1);
  }

//右侧南极高电平：？？？
  else if (L_S == LOW && R_S ==HIGH){
	 printf("???");

	 t_left(0,0);
	 sleep(1);
  }
	 
  else {
     printf("ERROR ");
     t_stop(0);
    }
  }
  return 0;

}

