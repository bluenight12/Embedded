/*
 * KDG_1122.c
 *
 * Created: 2023-11-22 ¿ÀÈÄ 2:06:40
 * Author: ekehd
 */

#include <mega128a.h>
#include <delay.h>

#define CLK_HIGH PORTD |= 0x01
#define CLK_LOW PORTD &= 0xFe
#define DAT_HIGH PORTD |= 0x02
#define DAT_LOW PORTD &= 0xFD
#define LD_HIGH PORTD |= 0x04
#define LD_LOW PORTD &= 0xFB

typedef unsigned char U8;
typedef unsigned short U16;

U8 EINT4_FLAG_BIT = 0;

void Send_Data(U8);
void End_Data(void);
void Start_Send(void);

U8 D8 = 0;
U8 D9 = 0;

const U8 voltage[19] = {0x19, 0x33, 0x4D, 0x66, 0x80, 0x9A, 0xB3, 0xCD, 0xE6, 0xFF,
                        0xE6, 0xCD, 0xB3, 0x9A, 0x80, 0x66, 0x4d, 0x33, 0x19};
                       
void main(void)
{
    DDRD = 0x0F;
    EIMSK |= 0b00010000;
    EICRB |= 0b00000010;
    SREG |= 0x80;
    CLK_HIGH;
    while (1)
    { 
    Start_Send();           
        if(EINT4_FLAG_BIT == 1){
            EINT4_FLAG_BIT = 0;
            Start_Send();
        }    
    }
}

interrupt [EXT_INT4] void ext4_int(void)
{
    SREG &= 0x7F;
    EINT4_FLAG_BIT = 1;                                
    SREG |= 0x80;   
}

void Start_Send(){
    U8 i;
    U8 bit_i;
    U8 value_buffer;
    for(i = 0;i<19;i++){
        Send_Data(D9);
        Send_Data(D8);
        value_buffer = voltage[i];
        for(bit_i = 0;bit_i<8;bit_i++){
            if((value_buffer) & 0x80 == 0x80) Send_Data(1);
            else Send_Data(0);
            value_buffer <<= 1;    
        }
        End_Data();    
    }    
}

void Send_Data(U8 data){
    CLK_LOW;
    delay_us(5);
    if(data == 1)
        DAT_HIGH;
    else
        DAT_LOW;
    delay_us(5);
    CLK_HIGH;
    delay_us(5);
}
void End_Data(){
    LD_HIGH;
    delay_us(5);
    LD_LOW;
    delay_us(5);
}