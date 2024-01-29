/*
 * KDG_1114_1.c
 *
 * Created: 2023-11-14 ���� 11:30:53
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
    
                                                            
    TIMSK = 0x41; //0�� ��Ʈ = overflow, 1����Ʈ = CTC
    TCNT0 = 131; // CNT�� �ʱ�ȭ
    TCCR0 = 0x06; // ���ֺ� ( 1 1 1 = 1024 ) , (3���� 6�� ��Ʈ�� over flow or CTC ���� �Ǵ�)
    TCCR2 = 0x05;
    //OCR0 = 155; // �񱳰� �ʱ�ȭ
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