// EEEN202 Staircase ADC
// Nick Thompson, Robin Dykstra 2018

#include "lcd.h"
#include <stdio.h>

//Macros
#define Sfr(x, y)       sfr x = y
#define Sbit(x, y, z)   sbit x = y^z

// Pin and Port definitions
Sfr(PORT_DAC,0x80);  //DAC output port sfr(NAME,ADDR) //Port 0
Sbit(PIN_CMP,0xB0,7); //Comparator input pin sbit(NAME,ADDR,BIT_NUM) // Port 3 pin 7


// Stair case converter code
unsigned char staircaseConverter(){
    unsigned char test_val;
    PORT_DAC = 0;
    unsigned char res = 0;
    unsigned char val = 255;
    for(test_val=7; test_val>=0; test_val--){
        val /= 2;
        PORT_DAC = res + val; //set the value of the port
        delay(100); // wait for the comparator to stabilise
        if(PIN_CMP){ //check if the comparator is high
            res = PORT_DAC;
        }
    }
    PORT_DAC = 0;
    return test_val;  //return the value if the comparator is high
}


void main(){
    int adc_value;
    char output_text[16];
    initLCD();
    while(1){
        adc_value=staircaseConverter();
        sprintf(output_text,"V: %.2f", (adc_value * 0.03852 -4.9));
        writeLineLCD(output_text);
        delay(10000);
        clearLCD();
    }
}
