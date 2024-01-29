/*
 * KDG_1103_2.c
 *
 * Created: 2023-11-03 ¿ÀÈÄ 3:10:23
 * Author: ekehd
 */

#include <mega128a.h>
#include <delay.h>

#define ON 0
#define OFF 1

void main(void)
{
    unsigned char sw;
    unsigned char i;    
    DDRE &= 0x0F;
    DDRC = 0xFF;
    PORTC = 0xFF;
     
    while (1)
    {                
        sw = PINE & 0xF0;
        switch(sw){
            case 0xE0:
                PORTC = 0x00;
                break;
            case 0xD0:
                PORTC = 0xFF;
                break;
            case 0xB0:
                PORTC = 0xFF;
                PORTC = 0xAA;
                break;
            case 0x70:
                PORTC = 0xFF;
                PORTC = 0x55;
                break;     
            case 0xC0:
                PORTC ^= 0xFF;
                delay_ms(500);
                break;
            case 0x60:            
                PORTC = 0xFE;               
                break;
            
        }          

    }
}
