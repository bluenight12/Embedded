/*
 * KDG_1113_2.c
 *
 * Created: 2023-11-13 오전 11:38:56
 * Author: ekehd
 */

#include <mega128a.h>
#include <delay.h>

unsigned char EINT4_FLAG_BIT = 0;
unsigned char EINT5_FLAG_BIT = 0;
unsigned char EINT6_FLAG_BIT = 0;
unsigned char EINT7_FLAG_BIT = 0;
unsigned char led = 0xFE;

void main(void)
{
    DDRC = 0xFF;
    
    PORTC = 0xFF;
    EIMSK = 0b11110000;
    EICRB = 0b11101110;
    SREG |= 0x80;
    
    
    while (1)
    {
        if(EINT4_FLAG_BIT == 1){
            EINT4_FLAG_BIT = 0;
            led <<= 1;
            led |= 0x01;
            if(led == 0xFF) led = 0xFE;
            PORTC = led;
            delay_ms(500);
        }
        if(EINT5_FLAG_BIT == 1){
            EINT5_FLAG_BIT = 0;
            led >>= 1;
            led |= 0x80;
            if(led == 0xFF) led = 0x7F;
            PORTC = led;
            delay_ms(500);    
        }
        if(EINT6_FLAG_BIT == 1){
            EINT6_FLAG_BIT = 0;
        }
        if(EINT7_FLAG_BIT == 1){
            EINT7_FLAG_BIT = 0;
        }
    }
}

interrupt [EXT_INT4] void external_int4(void)
{
    SREG &= 0x7F;
    EINT4_FLAG_BIT = 1; //인터럽트의 상태 플래그를 하나 만들고 내부에서는 value만 변경
    SREG |= 0x80;
}

interrupt [EXT_INT5] void external_int5(void)
{
    SREG &= 0x7F;
    EINT5_FLAG_BIT = 1; 
    SREG |= 0x80;
}

interrupt [EXT_INT6] void external_int6(void)
{
    SREG &= 0x7F;
    EINT6_FLAG_BIT = 1; 
    SREG |= 0x80;                    
}

interrupt [EXT_INT7] void external_int7(void)
{
    SREG &= 0x7F;
    EINT7_FLAG_BIT = 1; 
    SREG |= 0x80;
}