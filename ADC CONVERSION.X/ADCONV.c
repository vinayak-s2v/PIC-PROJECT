/*
 * File:   ADCONV.c
 * Author: User
 *
 * Created on 13 March, 2024, 7:05 PM
 */


#include <xc.h>
#define _XTAL_FREQ 20000000


// CONFIG
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = ON         // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3/PGM pin has PGM function; low-voltage programming enabled)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)


#define RS PORTDbits.RD0
#define RW PORTDbits.RD1
#define EN PORTDbits.RD2

unsigned int a,b,c,d,e,f;
unsigned int adc;
float temperature, adc1;


void lcd_command(unsigned char cmd)
{
    PORTC = cmd;
    RS=0;
    RW=0;
    EN=1;
    __delay_ms(5);
    EN=0;
}

void lcd_data(unsigned char data)
{
    PORTC = data;
    RS=1;
    RW=0;
    EN=1;
    __delay_ms(5);
    EN=0;
}

void lcd_string(const unsigned char *str, unsigned char num)
{
    unsigned char i;
    for(i=0;i<num;i++)
    {
        lcd_data(str[i]);
    }
}
void lcd_initialise()
{
    lcd_command(0x38);
    lcd_command(0x06);
    lcd_command(0x0c);
    lcd_command(0x01);
}

void __interrupt() adc_conv()
{
    if(PIR1bits.ADIF==1)
    {
        adc=(ADRESH<<8);
        adc=adc+ADRESL;
        PIR1bits.ADIF=0;
    }
}


void main() 
{
    
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    PIE1bits.ADIE = 1;
    
    TRISC = 0X00;
    TRISD = 0X00;
    lcd_initialise();
   
    lcd_command(0x80);
    lcd_string("SENSOR:",7);
    
    ADCON0 = 0X41;
    ADCON1 = 0X8E;
    
    while(1)
    {
       ADCON0 = ADCON0 | (0X04);
       
       adc1 = adc / 2.046;
       
       temperature = adc1*100;
       
       lcd_command(0x8B);
       a = (int)temperature / 10;
       b = (int)temperature % 10;
       c = a % 10;
       d = a / 10;
       e = d % 10;
       f = d / 10;
       lcd_data(f+0x30);
       lcd_data(e+0x30);
       lcd_data('.');
       lcd_data(c+0x30);
       lcd_data(b+0x30);
    }
    return;
}
 
