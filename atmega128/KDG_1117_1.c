#include <mega128.h>
#include <delay.h>

void Putch(char); // 송신

unsigned char seg_pat[16]= {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d,
0x07,0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71};

void main(void)
{
    char string[] = "No I can't forget this evening, Or your face as you were leaving. But I guess that's just the way The story goes. You always smile but in your eyes. Your sorrow shows Yes it shows. No I can't forget tomorrow, When I think of all my sorrow, When I had you there, But then I let you go And now it's only fair. That I should let you know What you should know.";
    char *pStr; // 문자열 포인터
    delay_ms(7000); // PC에서 메시지 확인 위해
    UCSR0A = 0x0; // USART 초기화 (251p)
    UCSR0B = 0b00001000; // 송신 인에이블 TXEN = 1 (253p)
    UCSR0C = 0b00000110; // 비동기[7], 데이터 8비트 모드 (253,4p)
    UBRR0H = 0; // X-TAL = 16MHz 일때, BAUD = 9600
    UBRR0L = 103; // (257p)
    pStr = string;
    while(*pStr != 0) 
    {    
        Putch(*pStr);
        if(*pStr == 0x2C || *pStr == 0x2E){
            Putch(0x0D);
            *pStr++;                   
        }
        *pStr++;
    } // 문자열 전송
    while(1);
}

void Putch(char data) // 한 바이트 송신
{
    while((UCSR0A & 0x20) == 0x0); //UDRE0[5] = 1 송신준비완료 될 때까지 대기
    UDR0 = data; // 데이터 전송
}