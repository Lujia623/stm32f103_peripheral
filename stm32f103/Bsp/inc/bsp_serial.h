#ifndef __BSP_SERIAL_H__
#define __BSP_SERIAL_H__

#ifdef __cplusplus
extern "C"
{
#endif

// #ifdef _USART1
// #include "stm32f10x_usart.h"
// #endif
#include <stdio.h>
#include "stm32f10x_usart.h"

#define BAUD_RATE_2400                  2400
#define BAUD_RATE_4800                  4800
#define BAUD_RATE_9600                  9600
#define BAUD_RATE_19200                 19200
#define BAUD_RATE_38400                 38400
#define BAUD_RATE_57600                 57600
#define BAUD_RATE_115200                115200
#define BAUD_RATE_230400                230400
#define BAUD_RATE_460800                460800
#define BAUD_RATE_921600                921600
#define BAUD_RATE_2000000               2000000
#define BAUD_RATE_3000000               3000000

int fgetc(FILE* f);
int fputc( int ch, FILE *f);
void Send_Byte(USART_TypeDef *USARTx, uint16_t byte);
uint8_t Receive_Byte(USART_TypeDef *USARTx);
void SerialPortInit(unsigned long ulWantedBaud);

#ifdef __cplusplus
}
#endif

#endif/*__BSP_SERIAL_H__*/
