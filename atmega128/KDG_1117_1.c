#include <mega128.h>
#include <delay.h>

void Putch(char); // �۽�

unsigned char seg_pat[16]= {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d,
0x07,0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71};

void main(void)
{
    char string[] = "No I can't forget this evening, Or your face as you were leaving. But I guess that's just the way The story goes. You always smile but in your eyes. Your sorrow shows Yes it shows. No I can't forget tomorrow, When I think of all my sorrow, When I had you there, But then I let you go And now it's only fair. That I should let you know What you should know.";
    char *pStr; // ���ڿ� ������
    delay_ms(7000); // PC���� �޽��� Ȯ�� ����
    UCSR0A = 0x0; // USART �ʱ�ȭ (251p)
    UCSR0B = 0b00001000; // �۽� �ο��̺� TXEN = 1 (253p)
    UCSR0C = 0b00000110; // �񵿱�[7], ������ 8��Ʈ ��� (253,4p)
    UBRR0H = 0; // X-TAL = 16MHz �϶�, BAUD = 9600
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
    } // ���ڿ� ����
    while(1);
}

void Putch(char data) // �� ����Ʈ �۽�
{
    while((UCSR0A & 0x20) == 0x0); //UDRE0[5] = 1 �۽��غ�Ϸ� �� ������ ���
    UDR0 = data; // ������ ����
}