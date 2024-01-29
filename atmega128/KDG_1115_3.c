/*
 * KDG_1115_3.c
 *
 * Created: 2023-11-15 ¿ÀÈÄ 2:19:05
 * Author: ekehd
 */

#include <mega128a.h>

#define sbi(PORTX, bitX) PORTX |= (1<<bitX);
#define cbi(PORTX, bitX) PORTX &= ~(1<<bitX);

typedef unsigned char U8;
typedef unsigned int U16;

U16 pwm = 0x0200;
U8 EINT4_FLAG_BIT = 0;
U8 EINT5_FLAG_BIT = 0;  
U8 EINT6_FLAG_BIT = 0;
U8 EINT7_FLAG_BIT = 0;                                                                                                                                                   
void main(void)
{
    DDRB = 0xFF;
    DDRG = 0xFF;
    PORTG = 0x0F;
    EIMSK = 0b11110000;
    EICRB = 0b10101010;
    TIMSK = 0x00;
    TCCR1A = 0b00001011;
    TCCR1B = 0b00000100;
    TCCR1C = 0x00;
    OCR1CH = (pwm & 0xFF00) >> 8;
    OCR1CL = (pwm & 0x00FF); 
    SREG |= 0x80;
    while (1)
    {
        
        if(EINT4_FLAG_BIT == 1){
            EINT4_FLAG_BIT = 0;
            if(pwm < 0x03B0) pwm += 0x0040;
            OCR1CH = (pwm & 0xFF00) >> 8;
            OCR1CL = (pwm & 0x00FF);       
        }
        if(EINT5_FLAG_BIT == 1){
            EINT5_FLAG_BIT = 0;
            if(pwm > 0x0050) pwm -= 0x0040;
            OCR1CH = (pwm & 0xFF00) >> 8;
            OCR1CL = (pwm & 0x00FF);     
        }
        if(EINT6_FLAG_BIT == 1){
            EINT6_FLAG_BIT = 0;
            sbi(TCCR1A, 2);
        }
        if(EINT7_FLAG_BIT == 1){
            EINT7_FLAG_BIT = 0;
            cbi(TCCR1A, 2);
        }
    }
}

interrupt [EXT_INT4] void ext_int4(void)
{
    SREG &= 0x7F;
    EINT4_FLAG_BIT = 1;
    SREG |= 0x80;
}

interrupt [EXT_INT5] void ext_int5(void)
{
    SREG &= 0x7F;
    EINT5_FLAG_BIT = 1;
    SREG |= 0x80;
}

interrupt [EXT_INT6] void ext_int6(void)
{
    SREG &= 0x7F;
    EINT6_FLAG_BIT = 1;
    SREG |= 0x80;
}

interrupt [EXT_INT7] void ext_int7(void)
{
    SREG &= 0x7F;
    EINT7_FLAG_BIT = 1;
    SREG |= 0x80;
}