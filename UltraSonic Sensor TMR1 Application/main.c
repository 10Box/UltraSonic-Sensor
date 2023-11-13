#include <xc.h>
#include "config.h"
#define _XTAL_FREQ 4000000

float distance_global;
float calc_dist(void){
    TMR1 = 0;
    //send trigger to sensor
    RC0 = 1;
    __delay_us(10);
    RC0 = 0;
    //wait until you get input from the sensor
    while(!RC1);
    //once you get input you start counting the time it takes from start to on
    TMR1ON = 1;
    //wait until the input becomes 0
    while(RC1);
    //after that you need to turn off the timer 1 module because you calculated the time
    TMR1ON = 0;
    //take the value from timer 1 and multiply it my 1 x 10^-6 
    //why? because it takes one instruction cycle to make 1 tick in TMR1
    //and one instruction takes 4 clocks and since the crystal is 4MHZ so 4/4000000 = 1 x 10^-6
    //the reason why we multiply TMR1 x 1 x 10^-6 is to convert the number of ticks in TMR1 to time
    //ultrasonic sensor uses sound speed to travel and sound speed = 340 m/s
    //we need to convert it to cm so 340 x 100 = 34000
    //to get the distance between sensor and object >> speed = distance / time
    //so distance = speed * time = (34000 * TMR1 * 10^-6) / 2
    //the reason why we divide by 2 is because when the trigger is activated the echo pin gets activated
    //so the sensor transmits waves travelling by the speed of sound and when the waves hit an objects it bounces back
    //and the it hits the sensor again, so we need the distance traveled when it bounces back not when it transmits
    // so to summarize the number (34000 * TMR1 * 10^-6) / 2 = TMR1 / 58.823
    distance_global += TMR1 / 58.823;
    return distance_global;
}
void main(void) {
    unsigned char dist = 0;
    //making all RA GPIO pins to be outputs
    TRISA = 0x00;
    PORTA = 0x00;
    //using RC0 as output for the trigger and RC1 for input to receive the waves from echo pin
    TRISC0 = 0;
    TRISC1 = 1;
    TMR1IE = 1;
    PEIE = 1;
    GIE = 1;
    //configuring timer 1 module to timer mode
    TMR1CS = 0;
    T1CKPS0 = 0;
    T1CKPS1 = 0;
    while(1){
    dist = calc_dist() / 5;
    if(dist == 1) PORTA = 0x0F;
    else if(dist == 2) PORTA = 0x07;
    else if(dist == 3) PORTA = 0x03;
    else if(dist == 4) PORTA = 0x01;
    else PORTA = 0x00;
    __delay_ms(100);
    }
    return;
}

void __interrupt() ISR(){
    if(TMR1IF){
        distance_global += TMR1 / 58.823;
        TMR1IF = 0;
    }
}