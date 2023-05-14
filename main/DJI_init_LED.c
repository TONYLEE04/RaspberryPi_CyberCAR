#include <wiringPi.h>
#include <stdio.h>


#define LEDPin 21
int main()
{
    printf ("Raspberry Pi - Getboard Blink\n") ;

    wiringPiSetup () ;

    pinMode (LEDPin, OUTPUT) ;

    for ( ; ; )
    {
        digitalWrite (LEDPin, 1) ;
        delay (737);
        digitalWrite (LEDPin, 0) ;
        delay (200);
        digitalWrite (LEDPin, 1) ;
        delay (120);
        digitalWrite (LEDPin, 0) ;
        delay (120);
        digitalWrite (LEDPin, 1) ;
        delay (120);
        digitalWrite (LEDPin, 0) ;
        delay (500);
    }
    return 0;
}
