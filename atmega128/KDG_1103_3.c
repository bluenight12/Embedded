#include <mega128.h>
/*
 * KDG_1103_3.c
 *
 * Created: 2023-11-03 ¿ÀÈÄ 4:33:47
 * Author: ekehd
 */
    
const unsigned char led[8] =
{0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F};
 
void main(void)
{
    unsigned char state = 0;
    unsigned char o_sw , n_sw;
    
    DDRC = 0xFF;
    DDRE &= 0x0F;
    
    PORTC = led[state];
    o_sw = PINE & 0x10;
    
    while(1){
        n_sw = PINE & 0x10;
        if(o_sw != 0 && n_sw == 0){
            state = (state+1) % 8;                                       
        }
        else if(o_sw == 0 && n_sw != 0){
            state = (state+2) % 8;
        }
        PORTC = led[state]; 
        o_sw = n_sw;
    }
}