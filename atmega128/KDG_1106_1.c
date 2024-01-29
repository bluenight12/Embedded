
/*
 * KDG_Project1.c
 *
 * Created: 2023-11-06 오후 3:23:02
 * Author: ekehd
 */                       
 
#include <mega128a.h>
#include <delay.h>

#define CLK_HIGH    PORTD |= 0x01   // PD.0=1
#define CLK_LOW     PORTD &= 0xFE   // PD.0=0
#define DAT_HIGH    PORTD |= 0x02   // PD.1=1
#define DAT_LOW     PORTD &= 0xFD   // PD.1=0
    
#define SCL_OUT     DDRD |= 0x01    // PD.0=1
#define SDA_IN      DDRD &= 0xFD    // PD.1=0
#define SDA_OUT        DDRD |= 0x02    // PD.1=1

#define GET_BUTTON  PINE & 0xF0   // 4 Button of 1
#define MAX 10
typedef unsigned char U8;

U8 DEV_ADD_W = 0xA0;
U8 DEV_ADD_R = 0xA1;
U8 IIC_ADD = 0;
U8 IIC_DAT = 0;
U8 READ_DATA = 0;
U8 BUTTON_DATA = 0;

void IIC_START(void);
void IIC_STOP(void);
void ACK_write(void);
void no_ACK(void);
void ACK_read(void);
void Process_8data(U8 value);
void Process_8data_read(void);
void Byte_write(U8 value);
void Byte_arr_write(void);
void Random_read(void);
void Change_LED(U8 value);

U8 Data_arr[MAX] = {0x12, 0x31, 0xD4, 0xF2, 0xE5, 0x72, 0xAA, 0xB2, 0x64, 0x34};

void main(){

    DDRC = 0xFF;
    PORTC = 0xFF; 
    DDRE = 0x00;
    SCL_OUT;
    SDA_OUT;
    
    while(1){
                
        BUTTON_DATA = GET_BUTTON;
        switch(BUTTON_DATA){
            case 0xE0:              
                // 1 byte write
                IIC_START();
                Process_8data(DEV_ADD_W);
                Process_8data(0x15);
                Byte_write(0xCF);
                IIC_STOP();
                delay_ms(100);
                break;
            case 0xD0 :
                // 10 bytes write
                IIC_START();    
                Process_8data(DEV_ADD_W);
                Process_8data(0x14);
                Byte_arr_write();
                IIC_STOP();
                break;
            case 0xB0 :
                //1 byte read
                IIC_START();
                Process_8data(DEV_ADD_W);
                Process_8data(0x15);   
                IIC_START();
                Process_8data(DEV_ADD_R);
                Process_8data_read();
                IIC_STOP();  
                delay_ms(100);
                break;
            case 0x70 :
                //10 bytes read
                break;
        }
      
    }
}

void IIC_START(void){
    SDA_OUT;  delay_us(6);
    DAT_HIGH; delay_us(6);
    CLK_HIGH; delay_us(6);
    DAT_LOW; delay_us(6);
    CLK_LOW;
}

void IIC_STOP(void){
    SDA_OUT; delay_us(6);
    DAT_LOW; delay_us(6);
    CLK_HIGH; delay_us(6);
    DAT_HIGH; delay_us(6);
    CLK_LOW; 
}

void Process_8data(U8 value){
    U8 bit_count;
    U8 value_buffer;                 
    SDA_OUT;                
    for(bit_count = 0;bit_count<8;bit_count++){
        value_buffer = value;
        if(((value_buffer >> (7-bit_count)) & 0x01)==0x01){
            DAT_HIGH;
        }
        else{
            DAT_LOW;
        }
        delay_us(6);
        CLK_HIGH; 
        delay_us(12);
        CLK_LOW;    
    }
    ACK_read();       
}
             
void ACK_read(void){
    U8 data;
    U8 i;
    U8 j;
    SDA_IN;     
    delay_us(6);          
    CLK_HIGH;
    for(i = 0 ; i < 10 ; i++){
        data = PIND & 0x02;
        if(data == 0x02){
            PORTC = 0x55;
            break;
        }   
    }
    if(data != 0x02)
        PORTC = 0xFE;                        
    delay_us(12);
    CLK_LOW;
    SDA_OUT;
}
                            
void Byte_write(U8 value){
    Process_8data(value);
    
}

void Byte_arr_write(void) {
    U8 arr_i;
    SDA_OUT;
    for(arr_i = 0 ; arr_i < MAX ; arr_i++){
        Process_8data(Data_arr[arr_i]);
    }
}

void Process_8data_read(void){
    U8 data = 0x00;
    U8 bit_count;
    U8 get_bit;
    SDA_IN;
    for(bit_count = 0; bit_count < 8; bit_count++){     
        delay_us(6);
        CLK_HIGH;       
        delay_us(12);
        CLK_LOW;
        get_bit = ((PIND & 0x02) == 0x02) ? 0x01 : 0x00;
        get_bit <<= (7-bit_count);
        data |= get_bit;
    }                        
    no_ACK();            
    PORTC = data; 
}

void ACK_write(void){
    SDA_OUT;              // 데이터 출력
    DAT_LOW;
    delay_us(6);    
    CLK_HIGH;  
    delay_us(12);
    CLK_LOW;      
}

void no_ACK(void){
    SDA_OUT;              // 데이터 출력
    DAT_HIGH;
    delay_us(6);    
    CLK_HIGH;  
    delay_us(12);
    CLK_LOW;      
}

void Random_read(void){
    
}