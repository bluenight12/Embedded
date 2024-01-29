/*
 * KDG_1114_1.c
 *
 * Created: 2023-11-14 오전 11:30:53
 * Author: ekehd
 */

#include <mega128a.h>

typedef unsigned char U8;

#define ON 0
#define OFF 1

U8 led = 0xFF;
U8 TIM0_OVF_FLAG_BIT = 0;
U8 TIM0_COMP_FLAG_BIT = 0;
U8 TIM2_OVF_FLAG_BIT = 0;
U8 TIM2_COMP_FLAG_BIT = 0;
unsigned short cnt = 0;

void main(void)
{
    
    DDRC = 0xFF;
    PORTC = led;
    
                                                            
    TIMSK = 0x41; //0번 비트 = overflow, 1번비트 = CTC
    TCNT0 = 131; // CNT값 초기화
    TCCR0 = 0x06; // 분주비 ( 1 1 1 = 1024 ) , (3번과 6번 비트로 over flow or CTC 여부 판단)
    TCCR2 = 0x05;
    //OCR0 = 155; // 비교값 초기화
    TCNT2 = 6; 
    SREG |= 0x80; // Interrupt On
    
    while (1)
    {
        if(TIM0_OVF_FLAG_BIT){
            TIM0_OVF_FLAG_BIT = 0;
            cnt++;
            if(cnt == 304){
                PORTC.1 ^= 1;
            }
            if(cnt == 608){
                cnt = 0;
                PORTC.0 ^= 1;  
            }   
        }
        if(TIM0_COMP_FLAG_BIT){
            TIM0_COMP_FLAG_BIT = 0; 
        }
        if(TIM2_OVF_FLAG_BIT){
            TIM2_OVF_FLAG_BIT = 0;
            //PORTC.7 ^= 1;
        }
    }
}

interrupt [TIM0_OVF] void timer0_ovf(void)
{
    TCNT0 = 131;
    SREG &= 0x7F;
    TIM0_OVF_FLAG_BIT = 1;
    SREG |= 0x80;
}

interrupt [TIM0_COMP] void timer0_comp(void)
{
    SREG &= 0x7F;
    TIM0_COMP_FLAG_BIT = 1;
    SREG |= 0x80;
}

interrupt [TIM2_OVF] void timer2_ovf(void)
{
    TCNT2 = 6;
    SREG &= 0x7F;
    TIM2_OVF_FLAG_BIT = 1;
    SREG |= 0x80;
}