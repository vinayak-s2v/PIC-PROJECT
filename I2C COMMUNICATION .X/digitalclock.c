/*
 * File:   digitalclock.c
 * Author: User
 *
 * Created on 26 March, 2024, 12:47 PM
 */


#include <xc.h>
#define _XTAL_FREQ 20000000

// CONFIG
#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bit (BOR disabled)
#pragma config LVP = OFF        // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3/PGM pin has PGM function; low-voltage programming enabled)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)


#define RS PORTBbits.RB0
#define RW PORTBbits.RB1
#define EN PORTBbits.RB2

#define DS1307_address  0xD0
#define second         (0x00)
#define minute         (0x01)
#define hour           (0x02)
#define day            (0x03)
#define date           (0x04)
#define month          (0x05)
#define year           (0x06)
#define control_reg    (0x07)

void DS1307_write(char _second, char _minute, char _hour, char _day, char _date, char _month, char _year);
void DS1307_read(char slave_address, char register_address);
void bcd_to_ascii(unsigned char value);
char decimal_to_bcd(unsigned char value);

void lcd_command(unsigned char cmd);
void lcd_data(unsigned char data);
void lcd_initialise();
void lcd_string(const unsigned char *str, unsigned char num);

//void delay(unsigned int Delay);


void lcd_command(unsigned char cmd)
{
    PORTD=cmd;
    RS=0;
    RW=0;
    EN=1;
    __delay_ms(5);
    EN=0;
}

void lcd_data(unsigned char data)
{
    PORTD=data;
    RS=1;
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

unsigned char __sec, __min, __hr, __day, __date, __month, __yr, __con;
void main()
{
    TRISC = 0xff;
    
    SSPADD = 49;
    SSPCON = 0x28;
    
    TRISD = 0x00;
    PORTD = 0x00;
    TRISB = 0x00;
    PORTB = 0x00;
    lcd_initialise();
    
    lcd_command(0x80);
    lcd_string("CLOCK:",6);
    lcd_command(0xC0);
    lcd_string("DATE:",5);
    
    DS1307_write(3,2,1,1,5,4,20);
    __delay_ms(300);

while(1)
{
        __delay_ms(20);
    DS1307_read(DS1307_address, 0);
    
    
}
    return;
}

void bcd_to_ascii(unsigned char value)
{
    unsigned char bcd;
    bcd = value;
    bcd = (bcd&(0xf0));
    bcd = (bcd>>4);
    bcd = (bcd|(0x30));
    lcd_data(bcd);
    
    bcd = value;
    bcd = (bcd&(0x0f));
    bcd = (bcd|(0x30));
    lcd_data(bcd);
}

char decimal_to_bcd(unsigned char value)
{
    unsigned char msb,lsb,hex;
    msb = value/10;
    lsb = value%10;
    hex = (unsigned char)((msb<<4)+lsb);
    return hex;
}

void DS1307_write(char _second, char _minute, char _hour, char _day, char _date, char _month, char _year)
{
    SSPCON2bits.SEN = 1;
    while(SSPCON2bits.SEN);
    PIR1bits.SSPIF = 0;
    
    SSPBUF = DS1307_address;
    while(!PIR1bits.SSPIF);
    PIR1bits.SSPIF = 0;
    if(SSPCON2bits.ACKSTAT)
    {
        SSPCON2bits.PEN = 1;
        while(SSPCON2bits.PEN );
       
        return;
    }
    
    SSPBUF = second;
    while(!PIR1bits.SSPIF);
    PIR1bits.SSPIF = 0;
    if(SSPCON2bits.ACKSTAT)
    {
      SSPCON2bits.PEN = 1;
       { while(SSPCON2bits.PEN );}
      return;  
    }
    
    SSPBUF = decimal_to_bcd(_second);
    while(!PIR1bits.SSPIF);
    PIR1bits.SSPIF = 0;
    
    SSPBUF = decimal_to_bcd(_minute);
    while(!PIR1bits.SSPIF);
    PIR1bits.SSPIF = 0;
    
    SSPBUF = decimal_to_bcd(_hour);
    while(!PIR1bits.SSPIF);
    PIR1bits.SSPIF = 0;
    
    SSPBUF = decimal_to_bcd(_day);
    while(!PIR1bits.SSPIF);
    PIR1bits.SSPIF = 0;
    
    SSPBUF = decimal_to_bcd(_date);
    while(!PIR1bits.SSPIF);
    PIR1bits.SSPIF = 0;
    
    SSPBUF = decimal_to_bcd(_month);
    while(!PIR1bits.SSPIF);
    PIR1bits.SSPIF = 0;
    
    SSPBUF = decimal_to_bcd(_year);
    while(!PIR1bits.SSPIF);
    PIR1bits.SSPIF = 0;
    
    SSPBUF = 0x00;
    while(!PIR1bits.SSPIF);
    PIR1bits.SSPIF = 0;
    
    SSPCON2bits.PEN = 1;
    while(SSPCON2bits.PEN);
    return;
}

void DS1307_read(char slave_address, char register_address)
{
    SSPCON2bits.SEN = 1;
    while(SSPCON2bits.SEN);
    PIR1bits.SSPIF = 0;
    
    SSPBUF = slave_address;
    while(!PIR1bits.SSPIF)
    PIR1bits.SSPIF = 0;
    if(SSPCON2bits.ACKSTAT)
    {
        SSPCON2bits.PEN = 1;
         while(SSPCON2bits.PEN );
        return;
    }
    
    SSPBUF = register_address;
    while(!PIR1bits.SSPIF);
    PIR1bits.SSPIF = 0;
    if(SSPCON2bits.ACKSTAT)
    {
        SSPCON2bits.PEN = 1;
          while(SSPCON2bits.PEN );
        return;
    }
    
    SSPCON2bits.RSEN = 1;
    while(SSPCON2bits.RSEN);
    PIR1bits.SSPIF = 0;
    
    SSPBUF = (slave_address + 1);
    while(!PIR1bits.SSPIF);
    PIR1bits.SSPIF = 0;
    if(SSPCON2bits.ACKSTAT)
    {
        SSPCON2bits.PEN = 1;
         while(SSPCON2bits.PEN );
        return;
    }
    
    SSPCON2bits.RCEN = 1;
    while(!SSPSTATbits.BF);
    __sec = SSPBUF;
    
    SSPCON2bits.ACKDT = 0;
    SSPCON2bits.ACKEN = 1;
    while(SSPCON2bits.ACKEN);
    
    SSPCON2bits.RCEN = 1;
    while(!SSPSTATbits.BF);
    __min = SSPBUF;
    
    SSPCON2bits.ACKDT = 0;
    SSPCON2bits.ACKEN = 1;
    while(SSPCON2bits.ACKEN);
    
    SSPCON2bits.RCEN = 1;
    while(!SSPSTATbits.BF);
    __hr = SSPBUF;
    
    SSPCON2bits.ACKDT = 0;
    SSPCON2bits.ACKEN = 1;
    while(SSPCON2bits.ACKEN);
    
    SSPCON2bits.RCEN = 1;
    while(!SSPSTATbits.BF);
    __day = SSPBUF;
    
    SSPCON2bits.ACKDT = 0;
    SSPCON2bits.ACKEN = 1;
    while(SSPCON2bits.ACKEN);
    
    SSPCON2bits.RCEN = 1;
    while(!SSPSTATbits.BF);
    __date = SSPBUF;
    
    SSPCON2bits.ACKDT = 0;
    SSPCON2bits.ACKEN = 1;
    while(SSPCON2bits.ACKEN);
    
    SSPCON2bits.RCEN = 1;
    while(!SSPSTATbits.BF);
    __month = SSPBUF;
    
    SSPCON2bits.ACKDT = 0;
    SSPCON2bits.ACKEN = 1;
    while(SSPCON2bits.ACKEN);
    
    SSPCON2bits.RCEN = 1;
    while(!SSPSTATbits.BF);
    __yr = SSPBUF;
    
    SSPCON2bits.ACKDT = 0;
    SSPCON2bits.ACKEN = 1;
    while(SSPCON2bits.ACKEN);
    
    SSPCON2bits.RCEN = 1;
    while(!SSPSTATbits.BF);
    __con = SSPBUF;
    
    SSPCON2bits.ACKDT = 1;
    SSPCON2bits.ACKEN = 1;
    while(SSPCON2bits.ACKEN);
    
    SSPCON2bits.PEN = 1;
    while(SSPCON2bits.PEN);
    PIR1bits.SSPIF=0;
    
    lcd_command(0x88);
    bcd_to_ascii(__hr);
    lcd_data(':');
    bcd_to_ascii(__min);
    lcd_data(':');
    bcd_to_ascii(__sec);
    lcd_command(0xc7);
    bcd_to_ascii(__date);
    lcd_data('/');
    bcd_to_ascii(__month);
    lcd_data('/');
    bcd_to_ascii(__yr);
    
 }

//void delay(unsigned int Delay)
//{
//    while(Delay--);
//}
