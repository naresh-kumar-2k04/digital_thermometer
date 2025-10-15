
// PIC16F887 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = ON       // Power-up Timer Enable bit (PWRT enabled)
#pragma config MCLRE = ON       // RE3/MCLR pin function select bit (RE3/MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = ON        // Internal External Switchover bit (Internal/External Switchover mode is enabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is enabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#define _XTAL_FREQ 4000000
int high,low,value,mode=0,i;
int celcius,farenheit,kelvin;
void delay( int n)
{
    while(n--);
}
void delay_msg(unsigned long int a)
{
    while(a--);
}
void lcd(char rs,char pin)
{
    RE0=rs;
    PORTD=pin;
    RE1=1;
    __delay_ms(10);
    RE1=0;
    __delay_ms(10);
}
void string(char *ptr)
{
    while(*ptr)
    {
        TXREG=*ptr++;
        while(TXIF==0);
        TXIF=0;
    }
}
void wait()
{
    while(TXIF==0); 
    TXIF=0;
}
void gsm()
{
    //__delay_ms(1000);
    string("AT\r");
    for(i=0;i<20;i++)
    {
        delay(200000);
    }
    string("ATD+918190096940;\r");
    for( i=0;i<250;i++)
    {
        delay(200000);
    }
    string("ATH\r");
    if((kelvin<=311)||(celcius<=39)||(farenheit<=101))
    {
        RB3=0;
    }
}
void gsm_msg()
{
            string("AT\r");   //start of gsm sending
            for(i=0;i<5;i++)
            {
                delay_msg(20000);
            }
            string("AT+CMGF=1\r");   //call r message
            for(i=0;i<5;i++)
            {
                delay_msg(20000);
            }
            RA1=1;
            string("AT+CMGS=\"+918190096940\"\r");    //mobile number
            for(i=0;i<5;i++)
            {
                delay_msg(20000);
            }
            string("danger\r"); //message needs to send
            for(i=0;i<5;i++)
            {
                delay_msg(20000);
            }  
            TXREG=26;             //shortcut for ctrl+z
            while(TXIF==0);
            TXIF=0;
            RA1=0;
          /*  if((kelvin<=311)||(celcius<=39)||(farenheit<=101))
            {
                RB3=0;
            }*/
}
/*void esp(int c, int f, int k) 
{
    string("DATA:");
    TXREG = c / 100 + '0'; wait();
    TXREG = (c / 10) % 10 + '0';wait();
    TXREG = c % 10 + '0';wait();
    TXREG = ',';wait();

    TXREG = f / 100 + '0';wait();
    TXREG = (f / 10) % 10 + '0';wait();
    TXREG = f % 10 + '0';wait();
    TXREG = ',';wait();

    TXREG = k / 100 + '0';wait();
    TXREG = (k / 10) % 10 + '0';wait();
    TXREG = k % 10 + '0';wait();
    TXREG = '\n';wait();
}*/

void adc()
{
    GO=1;
        while(GO);
        low=ADRESL;
        high=ADRESH;
        if(high==0)
        {
            value=low;
        }
        if(high==1)
        {
            value=low+256;
        }
        if(high==2)
        {
            value=low+512;
        }
        if(high==3)
        {
            value=low+768;
        }
        if(mode==1)
        {
            lcd(0,0x80);
            celcius= value * 0.1466275659824;
            lcd(1,(celcius/100)+48);
            lcd(1,(celcius/10)%10+48);
            lcd(1,(celcius%10)+48);
            lcd(1,'c');
           // esp(celcius, farenheit, kelvin);

            if(celcius>39)
            {
                RB3=1;
                gsm();
                gsm_msg();
            }
            else
            {
                RB3=0;          
            }
        }
        else if(mode==2)
        {
            lcd(0,0x80);
            farenheit=(value * 0.1466275659824 * 1.8)+32;
            lcd(1,(farenheit/100)+48);
            lcd(1,(farenheit/10)%10+48);
            lcd(1,(farenheit%10)+48);
            lcd(1,'f');
           // esp(celcius, farenheit, kelvin);
            if(farenheit>101)
            {
                RB3=1;
                gsm();
                gsm_msg();
            }
            else
            {
                RB3=0;
            }
        }
        else if(mode==3)
        {
            lcd(0,0x80);
            kelvin=(value * 0.1466275659824) + 273.15;
            lcd(1,(kelvin/100)+48);
            lcd(1,(kelvin/10)%10+48);
            lcd(1,(kelvin%10)+48);
            lcd(1,'k');
          //  esp(celcius, farenheit, kelvin);
            if(kelvin>311)
            {
                RB3=1;  
                gsm();
                gsm_msg();
            }
            else
            {
                RB3=0;
            }
        }
}
void main()
{
    PORTA=PORTC=PORTB=PORTD=PORTE=0X00;
    TRISA=TRISC=0X01;
    TRISB=0X07;
    TRISD=TRISE=0X00;
    ANSEL=0X01;
    ANSELH=0X00;
    ADCON0=0X81;
    ADCON1=0XB0;
    SPBRG=25;
    BRGH=1;
    SYNC=BRG16=0;
    SPEN=TXEN=1;
    lcd(0,0X38);
    lcd(0,0x0E);
    while(1)
    {
        if(RB0==1)
        {
            mode=1;
        }
        if(RB1==1)
        {
            mode=2;
        }
        if(RB2==1)
        {
            mode=3;
        }
        adc();
    }
}
