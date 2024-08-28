/*
 * File:   lcd.c
 * Author: User
 *
 * Created on 7 March, 2024, 1:30 PM
 */
// CONFIG
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = ON         // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3/PGM pin has PGM function; low-voltage programming enabled)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.


#include <xc.h>
#define _XTAL_FREQ 20000000

#define RS PORTBbits.RB0
#define RW PORTBbits.RB1
#define EN PORTBbits.RB2

void lcd_data(unsigned char data)
{
    PORTD=data;
    RS=1;
    RW=0;
    EN=1;
    __delay_ms(5);
    EN=0;
}

void lcd_command(unsigned char cmd)
{
    PORTD=cmd;
    RS=0;
    RW=0;
    EN=1;
    __delay_ms(5);
    EN=0;
}

void lcd_initialise()
{
    lcd_command(0x38);
    lcd_command(0x06);
    lcd_command(0x0c);
    lcd_command(0x01);
}

void lcd_string(const unsigned char *str, unsigned char num)
{
    unsigned char i;
    for(i=0;i<num;i++)
    {
        lcd_data(str[i]);
    }
}

void main(void) 
{
    TRISC = 0X00;
    TRISD = 0X00;
    lcd_initialise();
    while(1)
    {
        lcd_command(0x83);
        lcd_string("EMBEDDED",8);
        lcd_command(0xc3);
        lcd_string("SYSTEMS",7);
    }
    return;
}
