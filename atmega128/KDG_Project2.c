/******************************************************************************************/
/* Project :                                                                              */
/* Name :                                                                                 */
/* Modify : 2023-11-20 오후 2:40:28                                                        */
/* Date: ekehd                                                                            */
/* Description :                                                                          */
/******************************************************************************************/

#include <mega128a.h>
#include <delay.h>

typedef unsigned char U8;
typedef unsigned short U16;
typedef unsigned int U32;

#define sbi(PORTX,bitX) PORTX |= (1<<bitX)
#define cbi(PORTX,bitX) PORTX &= ~(1<<bitX)

U8 EINT4_FLAG_BIT = 0;      
U8 EINT5_FLAG_BIT = 0;
U8 EINT6_FLAG_BIT = 0;
U8 EINT7_FLAG_BIT = 0;
U8 TIM0_OVF_FLAG_BIT = 0;   //delay 함수 없이 하기 위해 cnt 증가로만 쓸 예정
U8 TIM1_COMP_FLAG_BIT = 0; 
U8 TIM2_OVF_FLAG_BIT = 0;
U8 ADC_INT_FLAG_BIT = 0;
U8 Digit_Mode = 0;
U8 cnt_temp = 0;
U8 PWM_cnt = 0;
U8 PC_PWM_cnt = 0;
flash U8 seg_pat[10]= {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f};

U16 cnt_timer0 = 0;
U16 cnt_timer2 = 0;
U16 cnt_timer1 = 0;
U16 birthday = 1217;

U32 val = 1996;
U32 voltage = 0;

float fvolt;

char PWM_End[] = "PWM STOPPED\r";
char PC_PWM_End[] = "PC PWM STOPPED\r";

char *pStr;

void AD_begin(void);
void AD_disp(void);
void Send_Volt(void);
void Putch(char);
void Send_UART_Data(char *p);
void EINT4_Reset(void);
void main(void)
{
    DDRB |= 0xF0;
    DDRD |= 0xF0;
    DDRG |= 0x0F;
    DDRC |= 0xFF;
    PORTC |= 0xFF;
    EIMSK |= 0b11110000; // EINT4~7 On
    EICRB = 0b10111011; // 4,6(rising) 5,7(falling)     
    sbi(TIMSK, TOIE0); // TOIE0 overflow mode
    TCNT0 = 131;
    sbi(TCCR0, CS02); sbi(TCCR0, CS01); // Timer0 분주비   
    sbi(UCSR0B,RXEN0);
    sbi(UCSR0B,RXCIE0);
    AD_begin();
    val = birthday;
    AD_begin();
    SREG |= 0x80;
    
    while (1)
    {    
        if(TIM0_OVF_FLAG_BIT == 1){ 
            TIM0_OVF_FLAG_BIT = 0;
            cnt_timer0++;
            if(cnt_timer0 > 500) 
            {
                cnt_timer0 = 0;
                if(PWM_cnt != 0){
                    PWM_cnt++;
                    if(PWM_cnt > 7){
                        PWM_cnt = 0;
                        pStr = PWM_End;
                        Send_UART_Data(pStr);
                        cbi(TCCR3B,CS32);
                    }   
                }
                if(PC_PWM_cnt != 0){
                    PC_PWM_cnt++;
                    if(PC_PWM_cnt > 8){
                        PC_PWM_cnt = 0;
                        pStr = PC_PWM_End;
                        Send_UART_Data(pStr);
                        cbi(TCCR3B,CS32);
                    }   
                }
            }
        }
        //External Interrupt 4 Execute 
        if(EINT4_FLAG_BIT == 1){
            //TIMER2 ON 분주비 256 TCNT2 180
            EINT4_FLAG_BIT = 0;
            sbi(TIMSK, TOIE2);
            TCNT2 = 180;
            sbi(TCCR2, CS22);
            sbi(ADMUX, MUX2); sbi(ADMUX, MUX1); sbi(ADMUX, MUX0);  // ADC 7 use
            sbi(ADCSRA, ADEN); sbi(ADCSRA, ADFR); sbi(ADCSRA, ADPS2); sbi(ADCSRA, ADPS1); sbi(ADCSRA, ADPS0); sbi(ADCSRA, ADSC); // ADC Enable             
        } 
        if(TIM2_OVF_FLAG_BIT == 1){
            TIM2_OVF_FLAG_BIT = 0;
            if(cnt_timer2 >= 1000){ 
                cnt_timer2 = 0; 
                while((ADCSRA&0x10) == 0);
                val = (int)ADCL + ((int)ADCH << 8);
                Digit_Mode = 1; 
                cnt_temp++; 
            }   
            cnt_timer2++;
            if(Digit_Mode == 1 && cnt_temp >= 10){
                EINT4_Reset();
            }       
        }
        //External Interrupt 4 End
        
        //External Interrput 5 Execute
        if(EINT5_FLAG_BIT == 1){
            EINT5_FLAG_BIT = 0;
            cnt_timer1 = 0;
            sbi(TIMSK, OCIE1A); // TIMER1 CTC MODE A
            sbi(TCCR1B, WGM12); sbi(TCCR1B, CS12); sbi(TCCR1B, CS10); // TCCR1B CTC Mode, Freescale 1024
            OCR1AH = 0x4A;  //OCR1A = 19015
            OCR1AL = 0x47;
            sbi(ADMUX, MUX2); sbi(ADMUX, MUX1); // MUX 00110 Get voltage
            sbi(ADCSRA, ADEN); sbi(ADCSRA, ADIE); // ADC Interrupt On 
        }
        if(TIM1_COMP_FLAG_BIT == 1){ 
            TIM1_COMP_FLAG_BIT = 0;
            if(cnt_timer1 >= 10){
                cnt_timer1 = 0;
                sbi(ADCSRA, ADSC);                
            }
            cnt_timer1++;               
        }
        if(ADC_INT_FLAG_BIT == 1){
            ADC_INT_FLAG_BIT = 0;
            Send_Volt();
        }
        //External Interrupt 5 End  
        
        //External Interrupt 6 Execute
        if(EINT6_FLAG_BIT == 1){ 
            EINT6_FLAG_BIT = 0;
            PWM_START();
            PWM_cnt = 1;
        }        
        //External Interrupt 6 End
        
        if(EINT7_FLAG_BIT == 1){
            //분주비 256, 8.5% 5570(0x15C2)
            EINT7_FLAG_BIT = 0;
            cbi(TCCR3B,CS32);
            cbi(TCCR3B,WGM32);
            sbi(TCCR3B,CS32);
            sbi(UCSR0B,RXEN0);
            sbi(UCSR0B,RXCIE0);
            PC_PWM_cnt = 1;   
        }
        AD_disp();
    }
}



interrupt [EXT_INT4] void ext_int4(void)
{
    SREG &= 0x7F;
    EINT4_FLAG_BIT = 1;
    SREG |= 0x80;
}

interrupt [EXT_INT5] void ext_int5(void)
{
    SREG &= 0x7F;
    EINT5_FLAG_BIT = 1;
    SREG |= 0x80;
}

interrupt [EXT_INT6] void ext_int6(void)
{
    SREG &= 0x7F;
    EINT6_FLAG_BIT = 1;
    SREG |= 0x80;
}

interrupt [EXT_INT7] void ext_int7(void)
{
    SREG &= 0x7F;
    EINT7_FLAG_BIT = 1;
    SREG |= 0x80;
}

interrupt [TIM0_OVF] void timer0_ovf(void)
{
    SREG &= 0x7F;
    TCNT0 = 131;
    TIM0_OVF_FLAG_BIT = 1;
    SREG |= 0x80;
}

interrupt [TIM1_COMPA] void timer1_comp(void)
{
    SREG &= 0x7F;
    TIM1_COMP_FLAG_BIT = 1;
    SREG |= 0x80;
}

interrupt [TIM2_OVF] void timer2_ovf(void)
{
    SREG &= 0X7f; 
    TCNT2 = 180;
    TIM2_OVF_FLAG_BIT = 1;
    SREG |= 0x80;
}

interrupt [ADC_INT] void adc_int(void)
{
    SREG &= 0x7F;
    ADC_INT_FLAG_BIT = 1;
    SREG |= 0x80;
}

interrupt [USART0_RXC] void usart0_rx(void)
{
    SREG &= 0x7F;
        
    SREG |= 0x80;
}

void Timer_Reset(void){
    Digit_Mode = 0;
    val = birthday;
    TCCR0 &= ~((1 << CS02) | (1 << CS01) | (1 << CS 00));
    TCCR1 &= ~((1 << CS12) | (1 << CS11) | (1 << CS 10));
    TCCR2 &= ~((1 << CS22) | (1 << CS21) | (1 << CS 20));
    TCCR3 &= ~((1 << CS32) | (1 << CS31) | (1 << CS 30));   
}

void EINT4_Reset(void){
    cnt_temp = 0;
    Digit_Mode = 0;
    val = birthday;
    cbi(TCCR2, CS22);                                       
    cbi(ADMUX, MUX2); cbi(ADMUX, MUX1); cbi(ADMUX, MUX0);   // ADC Mux disable
    cbi(ADCSRA, ADEN); cbi(ADCSRA, ADFR); cbi(ADCSRA, ADSC);// ADC Disable
}

void Send_Volt(void){
    U8 N100, N10, N1, buf;
    voltage = (int)ADCL + ((int)ADCH << 8);
    //UART를 통해 출력
    sbi(UCSR0B, TXEN0);
    sbi(UCSR0C, UCSZ01);
    sbi(UCSR0C, UCSZ00);
    UBRR0H = 0;
    UBRR0L = 8;  
    fvolt = (float)voltage * 5.0 / 1024;
    voltage = (int)(fvolt * 100);
    N100 = voltage / 100;
    buf = voltage % 100;
    N10 = buf / 10;
    N1 = buf % 10;
    while((UCSR0A & 0x20) == 0x0);
    UDR0 = N100 + 0x30;
    while((UCSR0A & 0x20) == 0x0);
    UDR0 = '.';
    while((UCSR0A & 0x20) == 0x0);
    UDR0 = N10 + 0x30;
    while((UCSR0A & 0x20) == 0x0);
    UDR0 = N1 + 0x30;
    while((UCSR0A & 0x20) == 0x0);
    UDR0 = 'V';
    while((UCSR0A & 0x20) == 0x0);
    UDR0 = '\r';
    cbi(UCSR0B, TXEN0);   
}

void PWM_START(){
    //분주비 256, 17% 11141 (0x2B85)
    DDRE.3 = 1;   // OC3C 출력 설정    
    sbi(TCCR3B, WGM32); 
    sbi(TCCR3A, WGM31);   // Fast PWM
    sbi(TCCR3A, COM3A1);  // 반전 파형
    sbi(TCCR3A, COM3A0);
    sbi(TCCR3B, CS32);    // FreeScale 256
    TCCR3C = 0x0;
    TCNT3H = 0x0;
    TCNT3L = 0x0;
    OCR3AH = 0x2B;
    OCR3AL = 0x85;
}
void PC_PWM_START(){    
    cbi(TCCR3B,CS32);
    cbi(TCCR3B,WGM32);
    sbi(TCCR3B,CS32);
    
}
void Send_UART_Data(char *p){
    UCSR0A = 0x0;
    sbi(UCSR0B, TXEN0);
    sbi(UCSR0C, UCSZ01);
    sbi(UCSR0C, UCSZ00);
    UBRR0H = 0;
    UBRR0L = 8;
    while(*p != 0) Putch(*p++);
    cbi(UCSR0B, TXEN0); 
}
void AD_disp(void){
    float fval;
    U32 ival, buf;
    U8 N1000,N100, N10, N1;
    switch(Digit_Mode){ // 생년월일과 전압 그리고 온도를 case문으로 나누기
        case 0:
            ival = val; 
            break;
        case 1:
            fval = (float)val * 5.0 / 1024.0;
            ival = (int)(fval * 1000);  //온도 일때
            break;
        case 2:  
            fval = (float)val * 5.0 / 1024.0;
            ival = (int)(fval * 10);  //전압 일때
            break;
        default:
            break;            
    }
    N1000 = ival / 1000; // 정수부 추출 
    buf = ival % 1000;
    N100 = buf / 100;
    buf = buf % 100;
    N10 = buf / 10; // 소수 첫째 자리 추출
    N1 = buf % 10; // 소수 둘째 자리 추출
    switch(cnt_timer0 % 4){
        case 0:   
            PORTG = 0b00001000; // 7-Seg DIG4 ON(PG3=1), 
            PORTD = ((seg_pat[N1 % 10] & 0x0F) << 4) | (PORTD & 0x0F);
            PORTB = (seg_pat[N1 % 10] & 0x70 ) | (PORTB & 0x0F);
            break;    
        case 1:
            PORTG = 0b00000100; // 7-Seg DIG3 ON(PG2=1)
            PORTD = ((seg_pat[N10 % 10] & 0x0F) << 4) | (PORTD & 0x0F);
            PORTB = (seg_pat[N10 % 10] & 0x70 ) | (PORTB & 0x0F);
            if(Digit_Mode == 1) PORTB |= 0x80;
            break;    
        case 2:
            PORTG = 0b00000010; // 7-Seg DIG2 ON(PG1=1)
            PORTD = ((seg_pat[N100 % 10] & 0x0F) << 4) | (PORTD & 0x0F);
            PORTB = (seg_pat[N100 % 10] & 0x70 ) | (PORTB & 0x0F);
            break;
        case 3:
            PORTG = 0b00000001; // 7-Seg DIG1 ON(PG0=1)
            PORTD = ((seg_pat[N1000 % 10] & 0x0F) << 4) | (PORTD & 0x0F);
            PORTB = (seg_pat[N1000 % 10] & 0x70 ) | (PORTB & 0x0F);
            break;
        default:
            break;
    }         
}

void AD_begin(void){
    U8 i = 0;
    U32 buf;
    U8 N1000,N100, N10, N1;
    for(i = 0 ; i < 100; i++){
        N1000 = val / 1000; // 정수부 추출  
        buf = val % 1000;
        N100 = buf / 100;
        buf = buf % 100;
        N10 = buf / 10; // 소수 첫째 자리 추출
        N1 = buf % 10; // 소수 둘째 자리 추출
        PORTG = 0b00001000; // 7-Seg DIG4 ON(PG3=1), 
        PORTD = ((seg_pat[N1 % 10] & 0x0F) << 4) | (PORTD & 0x0F);
        PORTB = (seg_pat[N1 % 10] & 0x70 ) | (PORTB & 0x0F);
        delay_ms(1);
        PORTG = 0b00000100; // 7-Seg DIG3 ON(PG2=1), 
        PORTD = ((seg_pat[N10 % 10] & 0x0F) << 4) | (PORTD & 0x0F);
        PORTB = (seg_pat[N10 % 10] & 0x70 ) | (PORTB & 0x0F);
        delay_ms(1);
        PORTG = 0b00000010; // 7-Seg DIG2 ON(PG1=1), 
        PORTD = ((seg_pat[N100 % 10] & 0x0F) << 4) | (PORTD & 0x0F);
        PORTB = (seg_pat[N100 % 10] & 0x70 ) | (PORTB & 0x0F);
        delay_ms(1);
        PORTG = 0b00000001; // 7-Seg DIG1 ON(PG0=1), 
        PORTD = ((seg_pat[N1000 % 10] & 0x0F) << 4) | (PORTD & 0x0F);
        PORTB = (seg_pat[N1000 % 10] & 0x70 ) | (PORTB & 0x0F);
        delay_ms(1);
    }         
}

void Putch(char data)
{
    while((UCSR0A & 0x20) == 0x0);
    UDR0 = data;
}