#ifndef UART_H
#define UART_H

#include <lpc17xx.h>

//#define UART_PCONP_ENABLE   (0x00000008)
//#define UART_LCR_SETUP      (0x00000083)
//#define UART_LCR_WORD_LEN8  (0x00000003)
//#define UART_FCR_ENABLE     (0x07)

void uart_config(void);
void uart_data(unsigned char d);
void uart_str(unsigned char *ptr);
void uart_num(unsigned int num);
unsigned char uart_rx(void);
void delay(unsigned int a);

void uart_config()
{
    LPC_SC->PCONP |= 0x00000008;   // UART0 peripheral enable
    LPC_PINCON->PINSEL0 = 0x00000050;
    LPC_UART0->LCR = 0x00000083;     // Enable divisor latch, parity disable, 1 stop bit, 8-bit word length
    LPC_UART0->DLM = 0X00;
    LPC_UART0->DLL = 0x1A;               // Select baud rate 9600 bps for 4Mhz
    LPC_UART0->LCR = 0x00000003; // Disable divisor latch
    LPC_UART0->FCR = 0X07;    // FIFO enable, RX FIFO reset, TX FIFO reset
}

void uart_data(unsigned char d)
{
    while (!(LPC_UART0->LSR & (1 << 5))); // Check THR is empty or not, use polling method
    LPC_UART0->THR = d;                    // Load the data to THR
    delay(10);
}

void uart_str(unsigned char *ptr)
{
    while (*ptr != '\0')
    {
        uart_data(*ptr);
        ptr++;
    }
}

unsigned char uart_rx()
{
    while (!(LPC_UART0->LSR & (1 << 0))); // Check RBR is received data or not
    return LPC_UART0->RBR;
}

void uart_num(unsigned int num)
{
    if (num)
    {
        uart_num(num / 10);
        uart_data(num % 10 + 0x30);
    }
}

void delay(unsigned int a)
{
    unsigned int i, j; // 100x 6000/60Mhz = 0.01s--
    for (i = 0; i < a; i++)
        for (j = 0; j < 6000; j++)
            ;
}

#endif // UART_H

