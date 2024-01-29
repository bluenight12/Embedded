#include <mega128.h>                              //라이브러리 헤더 파일 포함
#include <delay.h>                             

typedef unsigned char U8;                         //typedef 전처리문을 사용  
typedef unsigned short U16;                       //변수의 다양한 사용을 위해 main문밖 Global variable로 선언
typedef unsigned int U32;

const U8 seg_pat[10]= {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f};       //const를 이용해 배열 사용
U8 string_menu1[] = "The UART program mode";  
U8 string_menu2[] = "External Interrupt4 : No 4";  
U8 string_menu3[] = "External Interrupt4 : No 5";  
U8 string_menu4[] = "External Interrupt4 : No 6";  
U8 string_menu5[] = "External Interrupt4 : No 7";     
U8 string_menu6[] = "Please press the numbers :";     
                                                  
U8 EINT4_FLAG_BIT=0;                              //변수 설정
U8 EINT5_FLAG_BIT=0;
U8 EINT6_FLAG_BIT=0;
U8 EINT7_FLAG_BIT=0;

U8 TIMER1_CTC_FLAG_BIT=0;
U8 TIMER2_OVF_FLAG_BIT=0;
U16 TIMER2_OVF_COUNT=0;

U8 ADC_RESULT_FLAG=0;
U32 TEMP_ADC_VALUE=0;

U8 NA100, NA10, NA1;
U8 NV100, NV10, NV1;
U8 UART_Rx_data;
U8 UART_Voltage_Cnt=0;
U32 ad_value=0;
U8 *pStr;  

                                                  //함수 설정
void Seg4_out(U8 N1, U8 N10, U8 N100, U8 N1000);  //Segment 네자리 정수 출력 함수 정의(학번 앞자리)
void Seg4_out2(U8 N1, U8 N10, U8 N100, U8 N1000); //Segment 네자리 정수 출력 함수 정의(학번 뒷자리)
void EXIT6_PWM_M6(void);
void EXIT7_PWM_M2(void);
void EXIT4_TIMER2(void);
void EXIT5_TIMER1(void);

void UART_menu(void);
void UART_menu_press(void);
void Putch(char data);               			     // 송신
void UART_volt(void);
void AD_display(int value);
void SEG7_TEMP_DIS(int val);
             

void main(void){                             
    DDRC = 0xFF;    PORTC = 0xFF;              //작동 확인용으로 LED 출력 설정
    DDRB = 0xF0;                                 //학번 디스플레이를 위한 포트B 4,5,6,7 출력 설정(LED)
    DDRD = 0xF0;                                 //학번 디스플레이를 위한 포트D 4,5,6,7 출력 설정(LED)
    DDRG = 0x0F;                                 //학번 디스플레이를 위한 포트G 0,1,2,3 출력 설정(COM)      
 
    Seg4_out(0,2,0,2);                           //학번 앞자리 표시
    Seg4_out(1,5,2,2);                          //학번 뒷자리 표시
    PORTG=0x00; 

    SREG |= 0x80;                                //status resister bit7(MSB)만 1으로 설정하여 모든 인터럽트를 허용(global interrupt enable),OR을 사용하여 나머지비트는 그대로
	UART_menu();    
	
    while(1){    
    switch(UART_Rx_data){
      case 0x34 :	   	// SW1(E.4)이 눌리면 LED 모두 ON
        EIMSK = 0b00010000;                          //인터럽트 사용 여부 결정 레지스터로 INT4 enable
        EICRB = 0b10111011;                          //INT4,5,6,7은 B에 의해 Interrupt trigger 방식 설정(7fa,6ri,5fa,4ri)              
        UART_menu_press();
		UART_Rx_data=0;
		break;       
      case 0x35 :	   	// SW1(E.4)이 눌리면 LED 모두 ON
        EIMSK = 0b00100000;                          //인터럽트 사용 여부 결정 레지스터로 INT5 enable
        EICRB = 0b10111011;                          //INT4,5,6,7은 B에 의해 Interrupt trigger 방식 설정(7fa,6ri,5fa,4ri)              
        UART_menu_press();
		UART_Rx_data=0;
        break;
	  case 0x36 :	   	// SW1(E.4)이 눌리면 LED 모두 ON
        EIMSK = 0b01000000;                          //인터럽트 사용 여부 결정 레지스터로 INT6 enable
        EICRB = 0b10111011;                          //INT4,5,6,7은 B에 의해 Interrupt trigger 방식 설정(7fa,6ri,5fa,4ri)              
        UART_menu_press();
		UART_Rx_data=0;
        break;
	  case 0x37 :	   	// SW1(E.4)이 눌리면 LED 모두 ON
        EIMSK = 0b10000000;                          //인터럽트 사용 여부 결정 레지스터로 INT7 enable
        EICRB = 0b10111011;                          //INT4,5,6,7은 B에 의해 Interrupt trigger 방식 설정(7fa,6ri,5fa,4ri)              
        UART_menu_press();
		UART_Rx_data=0;
        break;
	  default:   
        break;
    } // end of switch  
	
    if(ADC_RESULT_FLAG!=0) SEG7_TEMP_DIS(TEMP_ADC_VALUE);            //온도 값이 나올때는 온도를 디스플레이함
           
    if(EINT4_FLAG_BIT==1) {                        //인터럽트가 발생하면     
            PORTC=0xfa;
            EINT4_FLAG_BIT=0;                          //0으로 다시 바꿔주고
            EXIT4_TIMER2();                            //EXIT4_TIMER2함수를 실행한다.
        } //end of if
        if(EINT5_FLAG_BIT==1) { 
            EINT5_FLAG_BIT=0;  
            UART_Voltage_Cnt=0;
            EXIT5_TIMER1();
        } //end of if
        if(EINT6_FLAG_BIT==1) { 
            EINT6_FLAG_BIT=0;
            EXIT6_PWM_M6();
        } //end of if
        if(EINT7_FLAG_BIT==1) { 
            EINT7_FLAG_BIT=0;
            EXIT7_PWM_M2();
        } //end of if  
        if(TIMER2_OVF_FLAG_BIT==1){                     //오버플로우 모드가 발생하면
            TIMER2_OVF_FLAG_BIT=0;                      //0으로 바꿔주고
            TIMER2_OVF_COUNT++;                         //1씩 더해간다  
            if(TIMER2_OVF_COUNT==868){                  //4.427sec(주기가 5.1msec이므로 868번)   
                TIMER2_OVF_COUNT=0;        
                ADC_RESULT_FLAG++;                      
                ADMUX = 7;                              // A/D 변환기 멀티플렉서 선택 레지스터 
                ADCSRA = 0xE2;                          // ADEN=1, ADSC=1, ADFR=1, 16MHz, 8분주  
                delay_us(100);                          // 변환시간이 길어져 발생하는 ADC오류의 방지를 위한 딜레이가 필요함 
                TEMP_ADC_VALUE = (int)ADCL + ((int)ADCH << 8);  //A/D 변환값 읽기
                if(ADC_RESULT_FLAG==10){ 
                  ADC_RESULT_FLAG%=10;                  // 10회동안만 온도 표시    
                  TCCR2 = 0b00000000;                         // normal모드, 프리스케일= 1024분주
                } //end of if  
            } //end of if
       } //end of if
       if(TIMER1_CTC_FLAG_BIT==1){
            TIMER1_CTC_FLAG_BIT=0;  
            ADMUX = 6;                                  //ADC6 단극성 입력 선택
            ADCSRA = 0x82;                              
            ADCSRA = 0xC2;                              // ADEN=1, 8분주 변환 시작
          
            while((ADCSRA & 0x10) == 0);                // ADIF=1이 될 때까지(Single)
            ad_value = (int)ADCL + ((int)ADCH << 8);    // A/D 변환값 읽기
            AD_display(ad_value);                       // A/D 변환값 표시    
            
            UART_Voltage_Cnt++;           
            if(UART_Voltage_Cnt<=10) UART_volt();       // 유아트 디스플레이를 위한 함수    
            else {
				UART_Voltage_Cnt=0;
				TCCR1B = 0b00001000;                        // mode4(CTC), clock off
				UART_menu();
			}	
   
       } //end of if
        
    } //end of while
} //end of main
    
void Seg4_out(U8 N1, U8 N10, U8 N100, U8 N1000)    //학번 앞자리 2020 디스플레이
{
    U8 i;
    for(i = 0;i <40 ;i++){                                          //40번이 될??까지 디스플레이
        PORTG = 0b00001000;                                         //맨 우측 7Segment DIG4(PG3=1)ON, 1자리 표시
        PORTD = ((seg_pat[N1] & 0x0F) << 4) | (PORTD & 0x0F);       //[0]표시를 위해 ABCD표시, PORTD 하위 4bits 변경되지 않게 | 
        PORTB = (seg_pat[N1] & 0x70 ) | (PORTB & 0x0F);             //[0]표시를 위해 EFG표시, PORTB 하위 4bits 변경되지 않게 | 
        delay_ms(2);                                                //구별을 위한 딜레이
        PORTG = 0b00000100;                                         //우측에서 두번째 7Segment DIG3(PG2=1)ON, 10자리 표시
        PORTD = ((seg_pat[N10] & 0x0F) << 4) | (PORTD & 0x0F); 
        PORTB = (seg_pat[N10] & 0x70 ) | (PORTB & 0x0F); 
        delay_ms(2); 
        PORTG = 0b00000010;                                         //우측에서 세번째 7Segment DIG2(PG1=1)ON, 100자리 표시
        PORTD = ((seg_pat[N100] & 0x0F) << 4) | (PORTD & 0x0F);     
        PORTB = (seg_pat[N100] & 0x70 ) | (PORTB & 0x0F); 
        delay_ms(2);
        PORTG = 0b00000001;                                         //우측에서 네번째 7Segment DIG1(PG0=1)ON, 1000자리 표시
        PORTD = ((seg_pat[N1000] & 0x0F) << 4) | (PORTD & 0x0F);    
        PORTB = (seg_pat[N1000] & 0x70 ) | (PORTB & 0x0F); 
        delay_ms(2);    
    } //end of for    
}//end of Seg4_out

void Seg4_out2(U8 N1, U8 N10, U8 N100, U8 N1000)   //학번 뒷자리 2251 디스플레이
{ 
        PORTG = 0b00001000; 
        PORTD = ((seg_pat[N1] & 0x0F) << 4) | (PORTD & 0x0F); 
        PORTB = (seg_pat[N1] & 0x70 ) | (PORTB & 0x0F);
        delay_ms(1);                                                //구별을 위한 최소한의 미세한 딜레이 
        PORTG = 0b00000100; 
        PORTD = ((seg_pat[N10] & 0x0F) << 4) | (PORTD & 0x0F); 
        PORTB = (seg_pat[N10] & 0x70 ) | (PORTB & 0x0F); 
        delay_ms(1); 
        PORTG = 0b00000010; 
        PORTD = ((seg_pat[N100] & 0x0F) << 4) | (PORTD & 0x0F); 
        PORTB = (seg_pat[N100] & 0x70 ) | (PORTB & 0x0F); 
        delay_ms(1);
        PORTG = 0b00000001; 
        PORTD = ((seg_pat[N1000] & 0x0F) << 4) | (PORTD & 0x0F); 
        PORTB = (seg_pat[N1000] & 0x70 ) | (PORTB & 0x0F); 
        delay_ms(1);    
} //end of Seg4_out2

interrupt [EXT_INT4] void external_int4(void){   //main CPU가 프로그램을 수행하고 있다가 interrupt가 발생하면 밑 서비스루틴을 실행,헤더파일에서 정의된 인터럽트 소스명 사용,함수명은 임의로 설정
    SREG &= 0x7F;                                //Nesting 방지를 위해 status resister bit7만 0으로 설정하여 모든 인터럽트를 차단(global interrupt disable)
    EINT4_FLAG_BIT=1;                            //interrupt 짧게 프로그래밍하기 위해 이렇게 설정함. 인터럽트4가 들어와 1이되면 while문 안에 설정해둔 동작을 실행
    SREG |= 0x80;                                //status resister bit7만 1으로 설정하여 모든 인터럽트를 허용(global interrupt enable)
}                                                      
 
interrupt [EXT_INT5] void external_int5(void){ 
    SREG &= 0x7F;
    EINT5_FLAG_BIT=1;     
    SREG |= 0x80;  
} 
                                
interrupt [EXT_INT6] void external_int6(void){ 
    SREG &= 0x7F;  
    EINT6_FLAG_BIT=1;  
    SREG |= 0x80;   
}

interrupt [EXT_INT7] void external_int7(void){ 
    SREG &= 0x7F;  
    EINT7_FLAG_BIT=1;       
    SREG |= 0x80;       
}

interrupt [TIM2_OVF] void TIMER2_OVF_int(void){ 
    SREG &= 0x7F;  
    TIMER2_OVF_FLAG_BIT=1; 
    TCNT2 = 174;                                 //초기값을 인터럽트 안에도 써줘야함
    SREG |= 0x80;         
}

interrupt [TIM1_COMPA] void TIMER1_CTC_int(void){ 
    SREG &= 0x7F;   
    TIMER1_CTC_FLAG_BIT=1;        
    SREG |= 0x80;         
}

void EXIT4_TIMER2(void){            
    TIMSK |= 0b01000000;                        // overflow interrupt enable
    TCCR2 = 0b00000101;                         // normal모드, 프리스케일= 1024분주
    TCNT2 = 174;                                // 174=5.184ms (학번뒷자리 51이라 8비트로 51ms불가)
                                    
} //end of EXIT4_TIMER2              
  
void EXIT5_TIMER1(void){
    TCCR1A = 0b00000000;                        // mode4
    TCCR1B = 0b00001101;                        // mode4(CTC), 프리스케일 = 1024분주  
    TCCR1C = 0x00;
          
    TCNT1H = 0x0;                               //레지스터 초기화
    TCNT1L = 0x0;                               //레지스터 초기화   
    OCR1AH= 0x1F;                               //7968,hex1F20로 나온 것.
    OCR1AL = 0x20;                              //510ms (5100ms 16비트로 불가하여 510ms로 설정)
    TIMSK |= 0x10;                              //compare match interrupt enable
                                   
} //end of EXIT5_TIMER1

void EXIT6_PWM_M6(void){           
    TCCR3A = 0b10000010;                        //OC3A(PE3)에 파형출력, mode6 
    TCCR3B = 0b00001100;                        //mode6(Fast PWM모드),256분주 - 122.07Hz 구동
    TCCR3C = 0x00;              
      
    TCNT3H = 0x0;                               //레지스터 초기화
    TCNT3L = 0x0;                               //레지스터 초기화    
    OCR3AH= 0x01;                               //260,hex104로 나온 것.
    OCR3AL = 0x04;                     
    
    DDRE |= 0x08;                               //TIMER3 PWM PIN PE3은 DDRE 3을 출력설정해야 PWM 파형이 출력됨
} //end of EXIT6_PWM_M6

void EXIT7_PWM_M2(void){
    TCCR3A = 0b10000010;                        //OC3A(PE3)에 파형출력, mode2 
    TCCR3B = 0b00000100;                        //mode2(PWM Phase correct 모드),256분주 - 61.035Hz
    TCCR3C = 0x00;              
      
    TCNT3H = 0x0;                               //레지스터 초기화
    TCNT3L = 0x0;                               //레지스터 초기화   
    OCR3AH= 0x01;                               //260,hex104로 나온 것.
    OCR3AL = 0x04; 
    DDRE |= 0x08;                               //PE3 출력 
} //end of EXIT7_PWM_M2

void SEG7_TEMP_DIS(int val){ 
    float fval;
    int ival, buf; 
         
    fval = (float)val * 5.0 / 1023.0;             // 온도값으로 변환
    ival = (int)(fval * 1000.0);                  // 반올림 후 정수화(소수 첫째자리까지)    
    
    NA100 = ival / 100;                           // 정수부 10의 자리 추출
    buf = ival % 100; 
    NA10 = buf / 10;                              // 한자리수 추출
    NA1 = buf % 10;                               // 소수 첫째 자리 추출 
    
    PORTG = 0b00001000;                           // PG3=1, 소수 첫째 자리
    PORTD = ((seg_pat[NA1] & 0x0F) << 4) | (PORTD & 0x0F);  //PORTD 하위 4bits 변경되지 않게 |
    PORTB = (seg_pat[NA1] & 0x70 ) | (PORTB & 0x0F);        //PORTB 하위 4bits 변경되지 않게 |
    delay_ms(1);
    PORTG = 0b00000100;                           // PG2=1
    PORTD = 0x00;                                   
    PORTB = 0x80;                                 // DP on(소수점) 
    delay_ms(1);
    PORTG = 0b00000010;                           // PG1=1, 정수 일의 자리      
    PORTD = ((seg_pat[NA10] & 0x0F) << 4) | (PORTD & 0x0F);
    PORTB = (seg_pat[NA10] & 0x70 ) | (PORTB & 0x0F); 
    delay_ms(1);   
    PORTG = 0b00000001;                           // PG0=1,정수 10의 자리
    PORTD = ((seg_pat[NA100] & 0x0F) << 4) | (PORTD & 0x0F);
    PORTB = (seg_pat[NA100] & 0x70 ) | (PORTB & 0x0F);                                                                                                                                           
    delay_ms(1); 
} //end of SEG7_TEMP_DIS


void AD_display(int value) { 
    float fvalue;
    int ivalue, buff; 

    fvalue = (float)value * 5.0 / 1024.0;        // 전압 값으로 변환
    ivalue = (int)(fvalue * 100.0 + 0.5);        // 반올림 후 정수화,(소수 둘째자리까지) 
    
    NV100 = ivalue / 100;                        // 정수부 추출
    buff = ivalue % 100; 
    
    NV10 = buff / 10;                            // 소수 첫째 자리 추출
    NV1 = buff % 10;                             // 소수 둘째 자리 추출
} //end of AD_display


void UART_volt(void){    
    UCSR0A = 0x0;               // USART 초기화 
    UCSR0B = 0b00001000;        // 송신 인에이블 TXEN = 1 
    UCSR0C = 0b00000110;        // 비동기[7], 데이터 8비트 모드 
    UBRR0H = 0;                 // X-TAL = 16MHz 일때, BAUD = 9600
    UBRR0L = 103;  
    
	while((UCSR0A & 0x20) == 0x0); UDR0 =0x0D;            // 1st ROW  	
    while((UCSR0A & 0x20) == 0x0);     //송신 버퍼가 비어지면 UDRE0[5]= 1이 될 때까지 대기       
     UDR0 =NV100+0x30;                 //위 추출 숫자 아스키코드로 데이터 전송
    while((UCSR0A & 0x20) == 0x0);          
     UDR0 ='.';                        //문자'.'전송
    while((UCSR0A & 0x20) == 0x0);          
     UDR0 =NV10+0x30;                  //위 추출 숫자 아스키코드로 데이터 전송
    while((UCSR0A & 0x20) == 0x0);          
     UDR0 =NV1+0x30;                   //위 추출 숫자 아스키코드로 데이터 전송
    while((UCSR0A & 0x20) == 0x0);          
     UDR0 ='V';                        //문자'V'전송
    while((UCSR0A & 0x20) == 0x0);          
     UDR0 =' ';                        //알아보기 쉽게 띄어쓰기
} //end of UART_volt

void UART_menu(void){
    UCSR0A = 0x0;               // USART 초기화 
    UCSR0B = 0b10011000;           // RXCIE0 [7] =1 : 수신 완료 인터럽트 enable
                                  // 송수신 enable TXEN0 [3] = 1, RXEN0 [4] =1
    UCSR0C = 0b00000110;          // 비동기 데이터 8비트 모드  
    UBRR0H = 0;                 // X-TAL = 16MHz 일때, BAUD = 9600    
    UBRR0L = 103;
 	delay_ms(3000);  
                  
    while((UCSR0A & 0x20) == 0x0); UDR0 =0x0D;            // 1st ROW  
    pStr = string_menu1;               
    while(*pStr != 0) Putch(*pStr++);                      // 문자열 전송

    while((UCSR0A & 0x20) == 0x0); UDR0 =0x0D;            // 1st ROW          
    pStr = string_menu2;               
    while(*pStr != 0) Putch(*pStr++);                      // 문자열 전송

    while((UCSR0A & 0x20) == 0x0); UDR0 =0x0D;            // 1st ROW 
    pStr = string_menu3;               
    while(*pStr != 0) Putch(*pStr++);                      // 문자열 전송
    
	while((UCSR0A & 0x20) == 0x0); UDR0 =0x0D;            // 1st ROW      
    pStr = string_menu4;               
    while(*pStr != 0) Putch(*pStr++);                      // 문자열 전송
    
	while((UCSR0A & 0x20) == 0x0); UDR0 =0x0D;            // 1st ROW 
    pStr = string_menu5;               
    while(*pStr != 0) Putch(*pStr++);                      // 문자열 전송
    
	while((UCSR0A & 0x20) == 0x0); UDR0 =0x0D;            // 1st ROW  
    pStr = string_menu6;               
    while(*pStr != 0) Putch(*pStr++);                      // 문자열 전송
}

void UART_menu_press(void){
	while((UCSR0A & 0x20) == 0x0); UDR0 =0x0D;            // 1st ROW  
    pStr = string_menu6;               
    while(*pStr != 0) Putch(*pStr++);                      // 문자열 전송
}	
void Putch(char data)           // 한 바이트 송신         
{
    while((UCSR0A & 0x20) == 0x0);      //UDRE0[5] = 1 송신준비완료 될 때까지 대기
    UDR0 = data;                             // 데이터 전송  
} 

interrupt [USART0_RXC] void usart0_rx(void)        // 수신 완료 인터럽트
{
    SREG &= 0x7F; 
    UART_Rx_data = UDR0;                                  // 수신      
    SREG |= 0x80;            
}




