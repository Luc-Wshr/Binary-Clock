/*
--------------------------------------------------------------------------------
File: Main.c
Title: Binary-clock with 
Controller: PIC18F4520
Frequency: 4 MHz (PLL disabled)
Compiler: XC8 ver. 2.32
MPLAB X 5.50
Version: 1.2
Date: 01.12.2021
Author: Luc-Wshr, AndreiCosovan, bkubon
--------------------------------------------------------------------------------
*/

#include "Definitions.h"
#include <stdio.h>

//----------------------- Define constant Values -------------------------------
#define TMR1ResetValue  65535       // Set maximal possible Value for Timer1 before the counting starts from zero again (16-Bit counter register)
#define ONE_MILISECOND      1000    //  

//----------------------- Set subroutine Definitions ------------------------
void display_binary(int num);

// Define Global Variables as integers (Miliseconds, seconds, minutes and hours)
int Milsecs = 0;
int Seconds = -1;
int Minutes = 0;
int Hours = 0;

//----------------------- Timer Interrupts ------------------------------------- 
void __interrupt(high_priority) timer_overflow_interrupt(void){
    // check for timer-1 interrupt flag
    if(TMR1IF){
        // reset timer-1
        TMR1IF = 0;
        TMR1 = TMR1ResetValue  - ONE_MILISECOND;
        
        // Increase clock time + Time-Overflow-Handler
        Milsecs++;
        if(Milsecs >= 1000){                 // If Milsecs gets equal or over 1 sec
            Milsecs = 0;                     // Reset Milsecs to 0
            Seconds++;                       // Add 1 sec to seconds
            if(Seconds >= 60){               // If seconds gets equal or over 60 seconds
                Seconds = 0;                 // Reset seconds to 0
                Minutes++;                   // Add 1 min to minutes
                if(Minutes >= 60){           // If minutes gets equal or over 60 minutes
                    Minutes = 0;             // Reset minutes to 0
                    Hours = (Hours + 1) % 24;// Add 1 hour to Hours and whole modulo 24 to avoid times over 24 hours
                }
            }
            
            // print clock to console every Seconds
            display_binary(Hours);
            printf(":");
            display_binary(Minutes);
            printf(":");
            display_binary(Seconds);
            printf("\n");
            
            // Output the Clock on the pins/leds
            LATA = (unsigned char)Seconds;      // Output seconds on PORTA
            LATB = (unsigned char)Minutes;      // Output minutes on PORTB
            LATC = (unsigned char)Hours;        // Output hours on PORTC

        }
    }

    return;
}

void main(void) {  
    //----------------------- USART --------------------------------------------
    // Define Outputs/Inputs for the clock
    TRISA = 0; // Set PORTA as Output for the seconds
    TRISB = 0; // Set PORTB as Output for the minutes
    TRISC = 0; // Set PORTC as Output for the hours

    // Reset all pins/leds
    LATA = 0;
    LATB = 0;
    LATC = 0;
  
    // Initialization for printing values
    TRISDbits.RD7 = 1;
    TRISDbits.RD6 = 1;
    TXSTAbits.TXEN = 1;
    RCSTAbits.SPEN = 1;
    
    printf("\n\n");
    
    //----------------------- INTERRUPT SETUP ----------------------------------
    
    // enable global interrupts
    INTCON = 0b11000000;

    // enable timer-1 interrupt
    PIE1bits.TMR1IE = 1;

    // setup timer
    T1CONbits.TMR1ON = 1;
    // reset timer
    TMR1IF = 0;
    TMR1 = TMR1ResetValue  - ONE_MILISECOND;
    
// IDLE
    while(1){
    }
    
    return;
}

void putch(unsigned char data) {    // needed for printf
    while(!PIR1bits.TXIF){          // wait until transmitter is ready
        continue;
    }
    TXREG = data;                   // send one character
}

//----------------------- USART over Terminal ----------------------------------
void display_binary(int num){
    // initialize output String
    char binary[] = "00000000";
    
    // edit String (dezimal to binary conversion)
    for(int i=0;i<8;i++){
        if(num % 2 == 1){
            binary[7 - i] = '1';
        }else{
            binary[7 - i] = '0';
        }
        num /= 2;
    }
    
    // output to console
    printf(binary);
    
    return;
}