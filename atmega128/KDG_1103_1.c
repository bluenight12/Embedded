/*
 * KDG_1103_1.c
 *
 * Created: 2023-11-03 오후 12:36:59
 * Author: ekehd
 */

#include <mega128a.h>
#include <delay.h>

#define ON 0
#define OFF 1

const unsigned char led_arr[7] = {0xFC, 0xF9, 0xF3,0xE7,0xCF,0x9F,0x3F};

void main(void)
{   //1,2 2,3 3,4 4,5 5,6 6,7 7,8 순으로 켜지게 해보기
    unsigned char led = 0xFC;
    unsigned char count = 0;
    unsigned char i = 1;
    DDRC = 0xFF; // 포트 C 출력 설정
    PORTC = 0xFF;
    PORTC.0 = ON;
    PORTC.2 = ON;
    PORTC.4 = ON;
    PORTC.6 = ON;        
            
    while(1){
        //for를 이용
        /*for(i = 0;i<7;i++){
            PORTC = led_arr[i];
            delay_ms(500);
        }*/    
        //count를 이용
        /*PORTC = led_arr[count];   //초기값을 배열로 만들어줌
        count+=i;               // 한칸씩 이동
        delay_ms(500);
        if(count >= 6)
            i = -1;
        else if(count <= 0)
            i = 1;         */
        
        //비트연산자를 이용
        /*PORTC = led;  
        if(i == 1){
            led = led << 1;
            led |= 0x01;
        }
        else{
            led = led >> 1;
            led |= 0x80;
        }
        if(led == 0xFC)
            i *= -1;
        else if(led == 0x3F)
            i *= -1;
        delay_ms(500);*/
        
        PORTC ^= 0xFF;
        delay_ms(500);                     
    }
} 