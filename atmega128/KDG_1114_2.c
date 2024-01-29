/*
 * KDG_1114_2.c
 *
 * Created: 2023-11-14 오후 2:43:15
 * Author: ekehd
 */

#include <mega128a.h>

typedef unsigned char U8;

U8 EINT4_FLAG_BIT = 0;
U8 EINT5_FLAG_BIT = 0;
U8 EINT6_FLAG_BIT = 0;
U8 EINT7_FLAG_BIT = 0;
U8 TIM2_COMP_FLAG_BIT = 0;
U8 TIM0_OVF_FLAG_BIT = 0;
U8 TIM2_CNT = 0;
U8 TIM0_CNT = 0;
U8 TIM2_LED_FLAG = 0;
U8 TIM0_LED_FLAG = 0;


void main(void)
{
    DDRC = 0xFF;        // LED Output
    PORTC = 0xFF;       // LED OFF
    EIMSK = 0xF0;       // Interrupt Mask Set
    EICRB = 0b10101110; // fallling , rising Set  
    
    TIMSK = 0b10000001; // Timer 2 comp, Timer 0 overflow
    
    TCNT0 = 131;
    OCR2 = 249;
    SREG |= 0x80;       // Global Interrupt On
    while (1)
    {
        if(EINT4_FLAG_BIT == 1){
            EINT4_FLAG_BIT = 0;
            TCCR2 = 0b00001101; // Timer 2 Freescale 1024 , Compare match ( 1 0 )
        }
        if(EINT5_FLAG_BIT == 1){
            EINT5_FLAG_BIT = 0;
            TCCR2 = 0b00001000; // Timer 2 Clock End
        }
        if(EINT6_FLAG_BIT == 1){
            EINT6_FLAG_BIT = 0;
            TCCR0 = 0b00000110; // Timer 0 Freescale 256 , Overflow ( 0 0 )
        }
        if(EINT7_FLAG_BIT == 1){
            EINT7_FLAG_BIT = 0; 
            TCCR0 = 0b00000000; // Timer 0 Clock End
        }
        if(TIM2_COMP_FLAG_BIT == 1){
            TIM2_COMP_FLAG_BIT = 0;
            TIM2_CNT++;
            if(TIM2_CNT == 44){ // 44 * 16 = 704
                TIM2_CNT = 0;
                if(TIM2_LED_FLAG == 0){
                    TIM2_LED_FLAG = 1;
                    PORTC = 0xAA;
                }
                else{
                    TIM2_LED_FLAG = 0;
                    PORTC = 0x55;    
                }
            }
        }
        if(TIM0_OVF_FLAG_BIT == 1){
            TIM0_OVF_FLAG_BIT = 0;
            TIM0_CNT++;
            if(TIM0_CNT == 65){ // 65 * 2 = 130
                TIM0_CNT = 0;
                if(TIM0_LED_FLAG == 0){
                    TIM0_LED_FLAG = 1;
                    PORTC = 0xF0;
                }
                else{
                    TIM0_LED_FLAG = 0;
                    PORTC = 0x0F;
                }
            }
        }
    }
}

interrupt [EXT_INT4] void external_int4(void)
{
    SREG &= 0x7F;
    EINT4_FLAG_BIT = 1;
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

interrupt [TIM2_COMP] void timer2_comp(void)
{
    SREG &= 0x7F;
    TIM2_COMP_FLAG_BIT = 1;
    SREG |= 0x80;
}

interrupt [TIM0_OVF] void timer0_ovf(void)
{
    TCNT0 = 131; //CNT 초기화      
    SREG &= 0x7F;
    TIM0_OVF_FLAG_BIT = 1;
    SREG |= 0x80;
}