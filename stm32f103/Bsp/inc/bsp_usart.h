#ifndef __BSP_USART_H__
#define __BSP_USART_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdarg.h>
#include <stdio.h>
#include "stm32f10x.h"
#include "bsp_serial.h"

#ifndef NUL
#define NUL 0x00
#endif

#define COMMON_TX_BUF_SIZE        512
#define COMMON_RX_BUF_SIZE        512
#define RT_CONSOLEBUF_SIZE        128

#define COMMON_USART_MAX          4

#define COMMON_NO_ERROR           0     //no error
#define COMMON_INV_CH_ERROR       1     //invalid channel
#define COMMON_TX_BUF_EMPTY       3     //tx buf is empty
#define COMMON_TX_BUF_FULL        4     //tx buf is full
#define COMMON_PARAM_INV          5     //param invalid

typedef enum
{
    COMMON_USART1,
    COMMON_USART2,
    COMMON_USART3,
    COMMON_USART4,
    COMMON_USARTX_MAX,
}eCommon_USARTx;

typedef struct
{
    uint32_t RingBufTxCtr;
    uint8_t *RingBufTxInPtr;
    uint8_t *RingBufTxOutPtr;
    uint8_t RingBufTx[COMMON_TX_BUF_SIZE];

    uint32_t RingBufRxCtr;
    uint8_t *RingBufRxInPtr;
    uint8_t *RingBufRxOutPtr;
    uint8_t RingBufRx[COMMON_RX_BUF_SIZE];
}sCommon_Ring_Buf;

void Common_init(eCommon_USARTx eCOMM_USARTx);
uint8_t Common_put_Tx_char(eCommon_USARTx eCOMM_USARTx, uint8_t byte);
uint8_t Common_put_Tx_data(eCommon_USARTx eCOMM_USARTx, const uint8_t *data, uint16_t data_len);
uint8_t Common_get_Tx_char(eCommon_USARTx eCOMM_USARTx,uint8_t *error);
void Common_Send_data(eCommon_USARTx eCOMM_USARTx,uint8_t *error);
void rt_kprintf(const char *fmt, ...);
void rt_hw_console_output( const char *str );
#ifdef __cplusplus
}
#endif

#endif /*__BSP_USART_H__*/
