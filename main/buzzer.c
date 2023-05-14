#include <wiringPi.h>
#include <softTone.h>
#include <stdio.h>

#define BuzPin    0


#define  CL1  131
#define  CL2  147
#define  CL3  165
#define  CL4  175
#define  CL5  196
#define  CL6  221
#define  CL7  248

#define  CM1  275
#define  CM2  307
#define  CM3  330
#define  CM4  350
#define  CM5  396
#define  CM6  441
#define  CM7  495

#define  CH1  525
#define  CH2  595
#define  CH3  661
#define  CH4  700
#define  CH5  800
#define  CH6  882
#define  CH7  990

int song_1[] = {CH1,CH2,CH5,0};

int beat_1[] = {12,14,16,10};


int song_2[] = {CL3,0,CL3,0,CL3,0,CL3,0,CL3,0,CL3,0,CL3,0};

int beat_2[] = {9,3,9,3,9,3,9,3,9,3,9,3,9,3,9,3,9,3};

int main(void)
{
	int i, j;

	if(wiringPiSetup() == -1){ //when initialize wiring failed,print messageto screen
		printf("setup wiringPi failed !");
		return 1; 
	}

	if(softToneCreate(BuzPin) == -1){
		printf("setup softTone failed !");
		return 1; 
	}

		printf("music is being played...\n");

		for(i=0;i<sizeof(song_1)/4;i++){
			softToneWrite(BuzPin, song_1[i]);	
			delay(beat_1[i] * 26);
		}
		delay(1250);
		while(1){
		for(j=0;j<sizeof(song_2)/4;j++){
			softToneWrite(BuzPin, song_2[j]);	
			delay(beat_2[j] * 26);
		}
}
	

	return 0;
}
