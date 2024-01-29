
 /*                                          */
 /* IIC_Program                              */
 /*                                          */
 /* Created: 2023-11-06 오후 3:23:02          */
 /* Author: Kam Da Gong                      */
 /* 1 Byte Write / Read                      */
 /* Sequence Write / Read                    */    
 
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
U8 Process_8data_read(void);
void Byte_write(U8 value);
void Byte_arr_write(void);
void Random_read(U8 value);
void Sequence_read(U8 value);

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
                Byte_write(0x0F);
                delay_ms(100);
                break;
            case 0xD0 :
                // 10 bytes write
                Byte_arr_write();
                break;
            case 0xB0 :
                //1 byte read
                Random_read(0x11);
                break;
            case 0x70 :
                //10 bytes read
                Sequence_read(0x21);
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
    delay_us(6);                
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
    IIC_START();                            
    Process_8data(DEV_ADD_W);
    Process_8data(0x11);
    Process_8data(value);
    IIC_STOP();                   
}

void Byte_arr_write(void) {
    U8 arr_i;
    IIC_START();    
    Process_8data(DEV_ADD_W);
    Process_8data(0x21);
    for(arr_i = 0 ; arr_i < MAX ; arr_i++){
        Process_8data(Data_arr[arr_i]);
    }
    IIC_STOP();
}

U8 Process_8data_read(){
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
    return data;  
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

void Random_read(U8 value){
    U8 data = 0x00;
    IIC_START();
    Process_8data(DEV_ADD_W);
    Process_8data(value);   
    IIC_START();
    Process_8data(DEV_ADD_R);
    data = Process_8data_read();                      
    no_ACK();            
    PORTC = data;
    IIC_STOP();       
}

void Sequence_read(U8 value){
    U8 data = 0x00;   
    U8 arr_i;
    IIC_START();
    Process_8data(DEV_ADD_W);
    Process_8data(value);   
    IIC_START();
    Process_8data(DEV_ADD_R);
    for(arr_i = 0; arr_i < MAX; arr_i++){
        data = Process_8data_read();                      
        no_ACK();            
        PORTC = data;
        delay_ms(500);                    
    }
    IIC_STOP();    
}