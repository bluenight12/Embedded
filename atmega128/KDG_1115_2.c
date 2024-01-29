/*
 * KDG_1115_2.c
 *
 * Created: 2023-11-15 ¿ÀÈÄ 12:13:19
 * Author: ekehd
 */

#include <mega128a.h>

typedef unsigned char U8;
typedef unsigned int U16;

#define sbi(PORTX,bitX)PORTX|=(1<<bitX)
#define cbi(PORTX,bitX)PORTX&= ~(1<<bitX)

U8 led = 0x55;
U8 TIM1_OVF_FLAG_BIT = 0;
U8 TIM3_COMP_FLAG_BIT = 0;


void main(void)
{
    DDRC = 0xFF;
    PORTC = 0xFF;
    sbi(TIMSK, TOIE1);
    ETIMSK = 0x10;
    TCCR1A = 0x00;
    TCCR1B = 0x04;
    TCCR1C = 0x00;
    TCCR3A = 0x00;
    sbi(TCCR3B, WGM32);
    sbi(TCCR3B, CS32);
    sbi(TCCR3B, CS30);
    TCCR3C = 0x00;
    TCNT1H = 0xB6;
    TCNT1L = 0xC2;
    OCR3AH = 0xB7;
    OCR3AL = 0x1A;
    SREG |= 0x80;
    
        
    while (1)
    {
        if(TIM1_OVF_FLAG_BIT == 1){
            TIM1_OVF_FLAG_BIT = 0;
            if(led == 0x0A)led = 0xA0;
            else led = 0x0A;
            PORTC = led;    
        }
        if(TIM3_COMP_FLAG_BIT == 1){
            TIM3_COMP_FLAG_BIT = 0;
            if(led == 0xF0)led = 0x0F;
            else led = 0xF0;
            PORTC = led;
        }
    }
}

interrupt [TIM1_OVF] void timer1_ovf(void)
{
    TCNT1H = 0xB6;
    TCNT1L = 0xC2;
    SREG &= 0x7F;
    TIM1_OVF_FLAG_BIT = 1;
    SREG |= 0x80;
}

interrupt [TIM3_COMPA] void timer3_comp(void)
{
    SREG &= 0x7F;
    TIM3_COMP_FLAG_BIT = 1;
    SREG |= 0x80;
}