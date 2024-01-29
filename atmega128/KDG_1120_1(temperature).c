/*
 * KDG_1120.c
 *
 * Created: 2023-11-20 오전 10:21:21
 * Author: ekehd
 */

#include <mega128a.h>
#include <delay.h>

void AD_disp(int);

typedef unsigned char u_char;

flash u_char seg_pat[10]= {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f};
u_char ADC_INT_FLAG_BIT = 0;


void main(void)
{
    int ad_val;
    DDRB = 0xF0;
    DDRD = 0xF0;
    DDRG = 0x0F;
    ADMUX = 0b00000111;
    ADCSRA = 0b10001111;
    delay_ms(5);    
    SREG |= 0x80;    
    while (1)
    {
        ADCSRA |= 0x40;
        if(ADC_INT_FLAG_BIT == 1){
            ADC_INT_FLAG_BIT = 0;
            ad_val = (int)ADCL + ((int)ADCH << 8);
        }
        
        AD_disp(ad_val);       
    }
}

interrupt [ADC_INT] void ADC_INTER(void)                                                                                                
{
    SREG &= 0x7F;
    ADC_INT_FLAG_BIT = 1;
    SREG |= 0x80;
}

void AD_disp(int val){
    float fval;
    int ival, buf;
    u_char N1000,N100, N10, N1;
    fval = (float)val * 5.0 / 1024.0; // 전압 값으로 변환
    ival = (int)(fval * 1000);
    N1000 = ival / 1000; // 정수부 추출  
    buf = ival % 1000;
    N100 = buf / 100;
    buf = buf % 100;
    N10 = buf / 10; // 소수 첫째 자리 추출
    N1 = buf % 10; // 소수 둘째 자리 추출
    PORTG = 0b00001000; // PG3=1, 소수 둘째 자리
    PORTD = ((seg_pat[N1] & 0x0F) << 4) | (PORTD & 0x0F);
    PORTB = (seg_pat[N1] & 0x70 ) | (PORTB & 0x0F);
    delay_ms(1);
    PORTG = 0b00000100; // PG2=1, 소수 첫째 자리
    PORTD = ((seg_pat[N10] & 0x0F) << 4) | (PORTD & 0x0F);
    PORTB = (seg_pat[N10] & 0x70 ) | (PORTB & 0x0F);
    PORTB = PORTB | 0x80; // DP on(소수점)
    delay_ms(1);
    PORTG = 0b00000010; // PG1=1, 정수부
    PORTD = ((seg_pat[N100] & 0x0F) << 4) | (PORTD & 0x0F);
    PORTB = (seg_pat[N100] & 0x70 ) | (PORTB & 0x0F);
    delay_ms(1);         
}