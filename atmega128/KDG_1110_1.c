/*
 * KDG_1110_1.c
 *
 * Created: 2023-11-10 ¿ÀÈÄ 2:26:39
 * Author: ekehd
 */

#include <mega128a.h>
#include <delay.h>

const unsigned char seg_arr[16] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x27, 
                                   0x7F, 0x67, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71};

const unsigned int birth_arr[8] = {1996, 9961, 9612, 6121, 1217, 2171,1719,7199};

#define Get_first_num(i) (seg_arr[i] >> 4)
#define Get_second_num(i) (seg_arr[i] & 0x0F) 

void seg_4digit(int num);

void main(void)
{
    char i, value, value2, z;  
    DDRB |= 0xF0;
    DDRD |= 0xF0;
    DDRG = 0x0F;
    
    PORTG = 0b00001000;
    PORTB = 0x00;
    PORTD = 0x00;           
    
    i = 0;
    while (1)
    {
    // Please write your application code here         
        seg_4digit(birth_arr[i]);  
        i++;
        i%=8;
         
    }    
}

void seg_4digit(int num){
    int N,N10,N100,N1000,remain,i;
    N1000 = num / 1000;
    remain = num % 1000;
    N100 = remain / 100;
    remain %= 100;
    N10 = remain / 10;
    N = remain % 10;
                    
    N1000 = seg_arr[N1000];
    N100 = seg_arr[N100];
    N10 = seg_arr[N10];
    N = seg_arr[N];
    for(i = 0;i<25;i++){
        PORTG = 0x08;
        PORTD = (N & 0x0F) << 4;
        PORTB = (N & 0xF0);
        delay_ms(5);
        PORTG = 0x04;
        PORTD = (N10 & 0x0F) << 4;
        PORTB = (N10 & 0xF0);
        delay_ms(5);
        PORTG = 0x02;
        PORTD = (N100 & 0x0F) << 4;
        PORTB = (N100 & 0xF0);
        delay_ms(5);
        PORTG = 0x01;
        PORTD = (N1000 & 0x0F) << 4;
        PORTB = (N1000 & 0xF0);
        delay_ms(5);
           
    }
}
           