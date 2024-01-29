#include <mega128.h> 
#include <delay.h> 

unsigned char EINT4_FLAG_BIT=0;  
unsigned char EINT5_FLAG_BIT=0;  
unsigned char EINT6_FLAG_BIT=0;  
unsigned char EINT7_FLAG_BIT=0;  

unsigned char TIMER2_COMPARE_INT_BIT =0;
unsigned char TIMER0_OVF_INT_BIT =0;

unsigned char BLINK2=0;
unsigned char BLINK0=0;

unsigned char USART0_INT_FLAG_BIT = 0;

char *pStr;
unsigned char menu_inp = 0;                                                 
unsigned char select_menu[] = "Menu Display\r";
unsigned char menu1[] = "1 : LED Blink\r";
unsigned char menu2[] = "2 : Write without you\r";
unsigned char select_number[] = "Press the number\r";

unsigned char song1[] = "No I can't forget this evening,\r";
unsigned char song2[] = "Or your face as you were leaving.\r";

void Putch(char);
void Show_display(void);

void main(void)
{ 
    DDRC = 0xFF; // 포트 C 출력 설정
    PORTC = 0xFF; // 포트 C에 초기값 출력
    
    EIMSK = 0b111110000; // 외부 인터럽트4 enable
    EICRB = 0b10101110; // 외부 인터럽트4 level trigger
    SREG |= 0x80; // 전역 인터럽트 enable-bit set
    
    
    while(1){   
        if(menu_inp == '1'){
            menu_inp = 0;
            if(PORTC == 0x00) PORTC = 0xFF;
            else PORTC = 0x00;
            Show_display();
        }
        if(menu_inp == '2'){
            menu_inp = 0;
            pStr = song1;
            while(*pStr != 0) Putch(*pStr++);
                        
            pStr = song2;
            while(*pStr != 0) Putch(*pStr++);
                        
            Show_display();    
        } 
        if(USART0_INT_FLAG_BIT == 1){
              
            USART0_INT_FLAG_BIT = 0;
                
            //menu_inp = menu_inp -'0'; 
        }                   
        if(EINT4_FLAG_BIT==1) {
            EINT4_FLAG_BIT=0;
            TCCR2 = 0x0D; // compare match 0704-7.04
            OCR2 = 109; 
            TCNT2 = 0x0; // 타이머/카운터0 레지스터 초기값
            TIMSK |= 0x80;
        }
        if(EINT5_FLAG_BIT==1) {
            EINT5_FLAG_BIT=0;
            TCCR2 &= 0xF8;
        }
        if(EINT6_FLAG_BIT==1) {
            EINT6_FLAG_BIT = 0;
            UCSR0A = 0x0;
            UCSR0B = 0b10011000;
            UCSR0C = 0b00000110;
            UBRR0H = 0;
            UBRR0L = 8;
            Show_display();
        }
        if(EINT7_FLAG_BIT==1) {
            EINT7_FLAG_BIT=0;
            TCCR0 &= 0xF8;
        }        
        
        if(TIMER2_COMPARE_INT_BIT==100) {
            TIMER2_COMPARE_INT_BIT=0;
            if(BLINK2==0) {
                PORTC=0xAA;
                BLINK2=1;
            }
            else {
                PORTC=0x55;
                BLINK2=0;                
            }
        }
        if(TIMER0_OVF_INT_BIT==100) {
            TIMER0_OVF_INT_BIT=0;
            if(BLINK0==0) {
                PORTC=0xF0;
                BLINK0=1;
            }
            else {
                PORTC=0x0F;
                BLINK0=0;                
            }    
        } 
           
    }
}

interrupt [EXT_INT4] void external_int4(void) 
{ 
    SREG &= 0x7F; // All Interrupt disable
    
    EINT4_FLAG_BIT=1;
    
    SREG |= 0x80; // All Interrupt enable
}
interrupt [EXT_INT5] void external_int5(void) 
{ 
    SREG &= 0x7F; // All Interrupt disable
    
    EINT5_FLAG_BIT=1;
    
    SREG |= 0x80; // All Interrupt enable
}
interrupt [EXT_INT6] void external_int6(void) 
{ 
    SREG &= 0x7F; // All Interrupt disable
    
    EINT6_FLAG_BIT=1;
    
    SREG |= 0x80; // All Interrupt enable
}
interrupt [EXT_INT7] void external_int7(void) 
{ 
    SREG &= 0x7F; // All Interrupt disable
    
    EINT7_FLAG_BIT=1;
    
    SREG |= 0x80; // All Interrupt enable
}

interrupt [TIM2_COMP] void timer_comp2(void) 
{ 
    TCNT2 = 0x0;
    SREG &= 0x7F; // All Interrupt disable
    TIMER2_COMPARE_INT_BIT++;
    SREG |= 0x80; // All Interrupt enable

}
interrupt [TIM0_OVF] void timer_ovf0(void) 
{ 
    TCNT0 = 175;
    SREG &= 0x7F; // All Interrupt disable
    TIMER0_OVF_INT_BIT++;
    SREG |= 0x80; // All Interrupt enable
}

interrupt [USART0_RXC] void usart0_rx(void)
{
    SREG &= 0x7F;
    USART0_INT_FLAG_BIT = 1; 
    menu_inp = UDR0;   
    SREG |= 0x80; 
}

void Putch(char data){
    while((UCSR0A & 0x20) == 0x0);
    UDR0 = data; 
}
void Show_display(){
    pStr = select_menu;
    while(*pStr != 0) Putch(*pStr++);
                
    pStr = menu1;
    while(*pStr != 0) Putch(*pStr++);
                
    pStr = menu2;
    while(*pStr != 0) Putch(*pStr++);
                
    pStr = select_number;
    while(*pStr != 0) Putch(*pStr++); 
}