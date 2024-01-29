/*
 * KDG_1115_4.c
 *
 * Created: 2023-11-15 오후 4:19:33
 * Author: ekehd
 */

#include <mega128a.h>
#include <delay.h>

typedef unsigned char U8;
typedef unsigned short U16;

U8 N1,N10,N100,N1000;
U8 seg_pat[10]= {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f};
U8 pos = 0;
U8 min = 0;
U8 sec = 0;
U16 cnt = 0;
U16 cnt2 = 0;

U8 EXT4_INT_FLAG_BIT, EXT5_INT_FLAG_BIT, EXT6_INT_FLAG_BIT, EXT7_INT_FLAG_BIT;
U8 TIM0_INT_FLAG_BIT, TIM2_INT_FLAG_BIT;

void Time_out(void);

void main(void)
{
    DDRB = 0xF0;
    DDRD = 0xF0;
    DDRG = 0x0F;
    DDRC = 0xFF;
    PORTC = 0xFF;    
    EIMSK = 0xF0;
    EICRB = 0xAA;  
    TCNT2= 131;
    TIMSK |= 0x40;
    TCCR2 = 0b00000100;    
    SREG |= 0x80;
    while (1)
    {   
        Time_out();
        if(cnt == 500){
            cnt = 0;
            sec--;
            if(sec == 0){
                
                if(min == 0){
                    TCCR0 = 0b00000000;
                    PORTC = 0x00;    
                }
                else{ 
                    min--;
                    sec = 60; 
                }
            }
        }
        if(cnt2 == 500){
            cnt2 = 0;
        } 
        
        if(EXT4_INT_FLAG_BIT == 1){
            EXT4_INT_FLAG_BIT = 0;
            N1 = sec % 10; // 현재 분 1자리 추출
            N10 = sec / 10; // 현재 분 10자리 추출
            N100 = min % 10; // 현재 시간 1자리 추출
            N1000 = min / 10; // 현재 시간 10자리 추출
            if(pos == 0) N1 = (N1 + 1) % 10; // 분 1단위 +1
            else if(pos == 1) N10 = (N10 + 1) % 6; // 분 10단위 +1
            else if(pos == 2) N100 = (N100 + 1) % 10;
            else N1000 = (N1000 + 1) % 10;
            min = N1000 * 10 + N100; // 시간 계산
            sec = N10 * 10 + N1;     
        }
        if(EXT5_INT_FLAG_BIT){
            EXT5_INT_FLAG_BIT = 0;
            pos = (pos + 1) % 4;
        }
        if(EXT6_INT_FLAG_BIT){
            EXT6_INT_FLAG_BIT = 0;
            TIMSK |= 0x01;
            TCCR0 = 0b00000110;
            TCNT0 = 131;         
        }
        if(EXT7_INT_FLAG_BIT){
            EXT7_INT_FLAG_BIT = 0;
            TCCR0 = 0b00000000;
        }
        if(TIM0_INT_FLAG_BIT){
            TIM0_INT_FLAG_BIT = 0;
            cnt++;
        } 
        if(TIM2_INT_FLAG_BIT == 1){
            TIM2_INT_FLAG_BIT = 0;
            cnt2++;
        }   

    }
}

interrupt [EXT_INT4] void ext_int4(void)
{
    SREG &= 0x7F;
    EXT4_INT_FLAG_BIT = 1;
    SREG |= 0x80;
}

interrupt [EXT_INT5] void ext_int5(void)
{
    SREG &= 0x7F;
    EXT5_INT_FLAG_BIT = 1;
    SREG |= 0x80;
}

interrupt [EXT_INT6] void ext_int6(void)
{
    SREG &= 0x7F;
    EXT6_INT_FLAG_BIT = 1;
    SREG |= 0x80;
}

interrupt [EXT_INT7] void ext_int7(void)
{
    SREG &= 0x7F;
    EXT7_INT_FLAG_BIT = 1;
    SREG |= 0x80;
}

interrupt [TIM0_OVF] void timer0_ovf(void)
{
    TCNT0 = 131;
    SREG &= 0x7F;
    TIM0_INT_FLAG_BIT = 1;
    SREG |= 0x80;
}

interrupt [TIM2_OVF] void timer2_ovf(void)
{
    TCNT2 = 131;
    SREG &= 0x7F;
    TIM2_INT_FLAG_BIT = 1;
    SREG |= 0x80;                                        
}
void Time_out(void)
{
    /*
    PORTG = 0b00001000; // 7-Seg DIG4 ON(PG3=1), 분 1자리 표시
    PORTD = ((seg_pat[sec % 10] & 0x0F) << 4) | (PORTD & 0x0F);
    PORTB = (seg_pat[sec % 10] & 0x70 ) | (PORTB & 0x0F);
    delay_us(500);
    PORTG = 0b00000100; // 7-Seg DIG3 ON(PG2=1), 분 10자리 표시
    PORTD = ((seg_pat[sec / 10] & 0x0F) << 4) | (PORTD & 0x0F);
    PORTB = (seg_pat[sec / 10] & 0x70 ) | (PORTB & 0x0F);
    delay_us(500);
    PORTG = 0b00000010; // 7-Seg DIG2 ON(PG1=1), 시간 1자리 표시
    PORTD = ((seg_pat[min % 10] & 0x0F) << 4) | (PORTD & 0x0F);
    PORTB = (seg_pat[min % 10] & 0x70 ) | (PORTB & 0x0F);
    delay_us(500);
    PORTG = 0b00000001; // 7-Seg DIG1 ON(PG0=1), 시간 10자리 표시
    PORTD = ((seg_pat[min / 10] & 0x0F) << 4) | (PORTD & 0x0F);
    PORTB = (seg_pat[min / 10] & 0x70 ) | (PORTB & 0x0F);
    delay_us(500);
    */
    switch(cnt2 % 4){
    case 0:   
        PORTG = 0b00001000; // 7-Seg DIG4 ON(PG3=1), 분 1자리 표시
        PORTD = ((seg_pat[sec % 10] & 0x0F) << 4) | (PORTD & 0x0F);
        PORTB = (seg_pat[sec % 10] & 0x70 ) | (PORTB & 0x0F);
        break;    
    case 1:
        PORTG = 0b00000100; // 7-Seg DIG3 ON(PG2=1), 분 10자리 표시
        PORTD = ((seg_pat[sec / 10] & 0x0F) << 4) | (PORTD & 0x0F);
        PORTB = (seg_pat[sec / 10] & 0x70 ) | (PORTB & 0x0F);
        break;    
    case 2:
        PORTG = 0b00000010; // 7-Seg DIG2 ON(PG1=1), 시간 1자리 표시
        PORTD = ((seg_pat[min % 10] & 0x0F) << 4) | (PORTD & 0x0F);
        PORTB = (seg_pat[min % 10] & 0x70 ) | (PORTB & 0x0F);
        break;
    case 3:
        PORTG = 0b00000001; // 7-Seg DIG1 ON(PG0=1), 시간 10자리 표시
        PORTD = ((seg_pat[min / 10] & 0x0F) << 4) | (PORTD & 0x0F);
        PORTB = (seg_pat[min / 10] & 0x70 ) | (PORTB & 0x0F);
        break;
    default:
        break;
    }
}