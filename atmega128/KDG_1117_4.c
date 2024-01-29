/*
 * KDG_1117_2.c
 *
 * Created: 2023-11-17 ¿ÀÈÄ 12:31:05
 * Author: ekehd
 */

#include <mega128a.h>

void main(void)
{     
    UCSR0A = 0x0;
    UCSR0B = 0b10011000;
    UCSR0C = 0b00000110;
    UBRR0H = 0;
    UBRR0L = 8;
    SREG |= 0x80;
    
    while(1);    
}

interrupt [USART0_RXC] void usart0_rx(void)
{
    unsigned char ch; 
    ch = UDR0; 
    if(ch >= 'a' && ch <= 'z') ch = ch - 'a' + 'A';
    else if(ch >= 'A' && ch <= 'Z') ch = ch - 'A' + 'a';
    while((UCSR0A & 0x20) == 0x0);
    UDR0 = ch;           
}