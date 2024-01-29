/*
 * KDG_1117_2.c
 *
 * Created: 2023-11-17 ¿ÀÈÄ 12:31:05
 * Author: ekehd
 */

#include <mega128a.h>

flash unsigned char seg_pat[16]= {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d,
0x07,0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71};

void main(void)
{
    unsigned char dd = 0;
    unsigned char rd = '0';
    
    DDRB = 0xF0;
    DDRD = 0xF0;
    DDRG = 0x0F;
    
    PORTG = 0x08;
    
    UCSR0A = 0x0;
    UCSR0B = 0b00010000;
    UCSR0C = 0b00000110;
    UBRR0H = 0;
    UBRR0L = 103;
    
    
    
    while (1)
    {
        while((UCSR0A & 0x80) == 0x0)
        {
            PORTD = ((seg_pat[dd] & 0x0F) << 4) | (PORTD & 0x0F);
            PORTB = (seg_pat[dd] & 0xF0) | (PORTB & 0x0F);    
        }                                                  
        rd = UDR0;
        
        if(rd >= '0' && rd <= '9') dd = rd - '0';
        else if(rd >= 'A' && rd <= 'F') dd = rd - 'A' + 10;
        else if(rd >= 'a' && rd <= 'f') dd = rd - 'a' + 10;
    }
}
