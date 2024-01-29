/*
 * KDG_1113_1.c
 *
 * Created: 2023-11-13 오전 8:57:08
 * Author: ekehd
 */

#include <mega128.h>
#include <delay.h>
                   
typedef unsigned char U8;

const U8 seg_pat[10]= {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f};

void Seg4_out(void);

U8 min, sec;
U8 hour = 12;
U8 EINT5_FLAG_BIT = 0;
U8 EINT6_FLAG_BIT = 0;
U8 EINT7_FLAG_BIT = 0;
U8 N1000, N100, N10, N1;
int i;
U8 pos = 0;

void main(void)
{    
    DDRB |= 0xF0;
    DDRD |= 0xF0;
    DDRG |= 0x0F;
    
    PORTB &= 0x0F;
    PORTD &= 0x0F;    
    SREG |= 0x80;
    EIMSK = 0b11100000;
    EICRB = 0b10101000;

    while (1)
    {                          
        Seg4_out();
        if(EINT5_FLAG_BIT == 1){
            EINT5_FLAG_BIT = 0;
            if(pos == 0) N1 = N1 == 0 ? 9 : N1-1;
            else if(pos == 1) N10 = N10 == 0 ? 9 : N10-1;
            else if(pos == 2) N100 = N100 == 0 ? 9 : N100-1;
            else N1000 = N1000 == 0 ? 9 : N1000-1;
             
        }
        if(EINT6_FLAG_BIT == 1){
            EINT6_FLAG_BIT = 0;
            N1 = min % 10;
            N10 = min / 10;
            N100 = hour % 10;
            N1000 = hour / 10;
            if(pos == 0) N1 = (N1 + 1) % 10;
            else if(pos == 1) N10 = (N10 + 1) % 6;
            else if(pos == 2) {
                if(N1000 == 2) N100 = (N100+1) % 4;
                else N100 = (N100 + 1) % 10;
            }
            else {
                if(N100 < 4) N1000 = (N1000 + 1) % 3;
                else N1000 = (N1000 + 1) % 2;
            }
            hour = N1000*10 + N100;
            min = N10*10+N1;
        }
        if(EINT7_FLAG_BIT == 1){
            EINT7_FLAG_BIT = 0;
            pos = (pos + 1) % 4;
        }
        if(i == 50) {
            i = 0;
            sec += 1;
            if(sec == 60){
                sec = 0;
                min = min + 1;
                if(min ==60){
                    min = 0;
                    hour = (hour +1)%24;
                }
            }
        }
        i++;
    }
}
interrupt [EXT_INT4] void external_int4(void)
{
    SREG &= 0x7F;
    //EINT4_FLAG_BIT = 1;
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

void Seg4_out(void) // 네 자리 정수 출력
{
    PORTG = 0b00001000;
    PORTD = ((seg_pat[min%10] & 0x0F) << 4) | (PORTD & 0x0F);
    PORTB = (seg_pat[min%10] & 0x70 ) | (PORTB & 0x0F); // E, F, G 표시
    if(pos == 0 && sec % 2 == 0) PORTB |= 0x80;
    else if(pos == 0 && sec % 2 == 1) PORTB &= 0x7F;
    //else PORTB &= 0x7F;
    delay_ms(5);
    PORTG = 0b00000100;
    PORTD = ((seg_pat[min/10] & 0x0F) << 4) | (PORTD & 0x0F);
    PORTB = (seg_pat[min/10] & 0x70 ) | (PORTB & 0x0F);
    if(pos == 1 && sec % 2 == 0) PORTB |= 0x80;
    else if(pos == 1 && sec % 2 == 1) PORTB &= 0x7F;
    delay_ms(5);
    PORTG = 0b00000010; // 7-Segment DIG2 ON(PG1=1)
    PORTD = ((seg_pat[hour%10] & 0x0F) << 4) | (PORTD & 0x0F);
    PORTB = (seg_pat[hour%10] & 0x70 ) | (PORTB & 0x0F);
    if(pos == 2 && sec % 2 == 0) PORTB |= 0x80;
    else if(pos == 2 && sec % 2 == 1) PORTB &= 0x7F;
    delay_ms(5);
    PORTG = 0b00000001; // 7-Segment DIG1 ON(PG0=1)
    PORTD = ((seg_pat[hour/10] & 0x0F) << 4) | (PORTD & 0x0F);
    PORTB = (seg_pat[hour/10] & 0x70 ) | (PORTB & 0x0F);
    if(pos == 3 && sec % 2 == 0) PORTB |= 0x80;
    else if(pos ==3 && sec % 2 == 1) PORTB &= 0x7F;
    delay_ms(5);
} 