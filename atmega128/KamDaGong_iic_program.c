
 /*                                          */
 /* IIC_Program                              */
 /*                                          */
 /* Created: 2023-11-06 ���� 3:23:02          */
 /* Author: Kam Da Gong                      */
 /* 1 Byte Write / Read                      */
 /* Sequence Write / Read                    */         
 /* Check sum                                */
 
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
typedef unsigned int U16;

U8 DEV_ADD_W = 0xA0;
U8 DEV_ADD_R = 0xA1;
U8 IIC_ADD = 0x11;
U8 IIC_DAT = 0;
U8 READ_DATA = 0;
U8 BUTTON_DATA = 0;
U16 RANDOM_ADD = 0x03FA;

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
void Change_ADD(void);

U8 check_sum = 0x00;
U8 read_sum = 0x00;
U8 Data_arr[MAX] = {0x12, 0x31, 0xD4, 0xFE, 0xC4, 0x15, 0xBB, 0xA1, 0xE4, 0x23};

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
                Byte_write(0x77);
                delay_ms(100);  //��ư�� �ѹ��� 2-3���� �������� ����Ǵ°��� ���� ���� �ݵ�� delay�� �ֱ�
                break;
            case 0xD0 :
                // 10 bytes write
                Byte_arr_write();
                delay_ms(100);
                break;
            case 0xB0 :
                //1 byte read
                Random_read(IIC_ADD);
                delay_ms(100);
                break;
            case 0x70 :
                //10 bytes read
                Sequence_read(0x21);  
                delay_ms(100);
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
    SDA_OUT;                 //���� ���
    delay_us(6);                
    for(bit_count = 0;bit_count<8;bit_count++){
        value_buffer = value;                // value�� ���� ��Ű�� �ʱ� ���� value_buffer ���
        if(((value_buffer >> (7-bit_count)) & 0x01)==0x01){   // MSB���� �����ͼ� ����
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
    delay_us(6);                   
    ACK_read();       
}
             
void ACK_read(void){
    U8 data;
    U8 i;
    SDA_IN;     
    delay_us(6);          
    CLK_HIGH;
    for(i = 0 ; i < 10 ; i++){        // AcK�� ���� ���� �𸣱� ������ for���� ���� ������ ���� �˻��Ѵ�
        data = PIND & 0x02;           // Port D 1 ������ ������ �޾Ƽ� 1 or 0 check
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
    //Change_ADD();
    Process_8data(DEV_ADD_W);   //Device Address
    //DEV_ADD_W = 0xA0;         // Change_ADD�� Device Address�� �ٲ��� �� �ʱ�ȭ
    Process_8data(IIC_ADD);     //Address
    Process_8data(value);       //Data
    IIC_STOP();                   
}
void Change_ADD(){      //���� 255�� �Ѿ �� ������ ���� �Լ�
    U8 REMAIN_ADD = 0x00;
    U8 bit_i;             
    if(((RANDOM_ADD >> 9) & 0x01) == 0x01){
        DEV_ADD_W |= 0x04;
        DEV_ADD_R |= 0x04;
    }
    if(((RANDOM_ADD >> 8)&0x01) == 0x01){
        DEV_ADD_W |= 0x02;
        DEV_ADD_R |= 0x02;
    }  
    /*for(bit_i = 0; bit_i < 8;bit_i++){
        if(RANDOM_ADD & 0x0001 == 0x0001) REMAIN_ADD |= 0x80;
        RANDOM_ADD >>= 1;
        REMAIN_ADD >>= 1;
    }*/   
    REMAIN_ADD = RANDOM_ADD;
    IIC_ADD = REMAIN_ADD;
}
void Byte_arr_write(void) {
    U8 arr_i;
    check_sum = 0; // check_sum �ʱ�ȭ 
    IIC_START();    
    Process_8data(DEV_ADD_W);  // Device Address 
    Process_8data(0x21);       // Address
    for(arr_i = 0 ; arr_i < MAX ; arr_i++){
        delay_us(6);
        Process_8data(Data_arr[arr_i]);  // Data
        check_sum += Data_arr[arr_i];    // Check_sum
    }
    IIC_STOP();
}

U8 Process_8data_read(){
    U8 data = 0x00;     // �ʱⰪ ����
    U8 bit_count;     //for���� �� ���� 
    U8 get_bit;     // �� bit�� 0���� 1���� ���� �� �� ���� 
    SDA_IN;        // ��Ʈ �����͸� ���� �غ�
    for(bit_count = 0; bit_count < 8; bit_count++){     
        delay_us(6);
        CLK_HIGH;       
        delay_us(12);
        CLK_LOW;                                            // Ŭ���� �÷ȴ� ������ �ݵ�� ����� �� Input ���� ���� �� �ִ�. �������Ϳ� ���� �Ǿ� �ִ� ���� �������⸸ �ϸ� ��
        get_bit = ((PIND & 0x02) == 0x02) ? 0x01 : 0x00;    
        get_bit <<= (7-bit_count);
        data |= get_bit;
    } 
    return data;  
}

void ACK_write(void){
    SDA_OUT;              // ������ ���
    DAT_LOW;              
    delay_us(6);    
    CLK_HIGH;             //Ŭ���� �ø��� ������ ACK (0) ����
    delay_us(12);
    CLK_LOW;      
}

void no_ACK(void){
    SDA_OUT;              // ������ ���
    DAT_HIGH;
    delay_us(6);    
    CLK_HIGH;  
    delay_us(12);         //Ŭ���� �ø��� ������ No ACK (1) ����
    CLK_LOW;     
}

void Random_read(U8 value){
    U8 data = 0x00;
    IIC_START();
    Process_8data(DEV_ADD_W);     // Device Address Write
    Process_8data(value);         // Address
    IIC_START();
    Process_8data(DEV_ADD_R);     // Device Address Read
    data = Process_8data_read();  // �� Read �ؿͼ� Data�� �ֱ�                    
    no_ACK();                     // 1����Ʈ�� �ޱ� ���� no Ack (1) ����
    PORTC = data;                 // LED�� data������ ����
    IIC_STOP();       
}

void Sequence_read(U8 value){
    U8 data = 0x00;   
    U8 arr_i;  
    read_sum = 0x00; // read sum �ʱ�ȭ 
    IIC_START();
    Process_8data(DEV_ADD_W);
    Process_8data(value);   
    IIC_START();
    Process_8data(DEV_ADD_R);
    for(arr_i = 0; arr_i < MAX; arr_i++){  
        data = Process_8data_read();                      
        ACK_write();            
        PORTC = data;
        delay_ms(500);
        PORTC = 0xFF;    
        delay_ms(500);  
        read_sum += data;              
    }         
    no_ACK();     
    if(check_sum == read_sum){              // sum ���� ���ٸ� ���� �����̱� 
        for(arr_i = 0; arr_i < 5; arr_i++){
            PORTC = 0xFF;
            delay_ms(500);
            PORTC ^= 0XFF;
            delay_ms(500);
        }
    }
    IIC_STOP();    
}