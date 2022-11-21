#include "bsp_usart.h"
#include "types.h"

static sCommon_Ring_Buf CommonRingBuf[COMMON_USART_MAX];

void Common_init(eCommon_USARTx eCOMM_USARTx)
{
    sCommon_Ring_Buf *pbuf;

    pbuf = &CommonRingBuf[eCOMM_USARTx];
    pbuf->RingBufTxCtr = 0;
    pbuf->RingBufTxInPtr = &pbuf->RingBufTx[0];
    pbuf->RingBufTxOutPtr = &pbuf->RingBufTx[0];
    pbuf->RingBufRxCtr = 0;
    pbuf->RingBufRxInPtr = &pbuf->RingBufRx[0];
    pbuf->RingBufRxOutPtr = &pbuf->RingBufRx[0];
}

uint8_t Common_put_Tx_char(eCommon_USARTx eCOMM_USARTx, uint8_t byte)
{
    sCommon_Ring_Buf *pbuf;

    if(eCOMM_USARTx >= COMMON_USARTX_MAX)
    {
        return COMMON_INV_CH_ERROR;
    }

    pbuf = &CommonRingBuf[eCOMM_USARTx];

    pbuf->RingBufTxCtr++;
    *pbuf->RingBufTxInPtr++ = byte;
    if(pbuf->RingBufTxInPtr == &pbuf->RingBufTx[COMMON_TX_BUF_SIZE])
    {
        pbuf->RingBufTxInPtr = &pbuf->RingBufTx[0];
    }

    return COMMON_NO_ERROR;
}

uint8_t Common_put_Tx_data(eCommon_USARTx eCOMM_USARTx, const uint8_t *data, uint16_t data_len)
{
    uint32_t i = 0;

    if(eCOMM_USARTx >= COMMON_USARTX_MAX)
    {
        return COMMON_INV_CH_ERROR;
    }

    if(data_len >= COMMON_TX_BUF_SIZE)
    {
        return COMMON_TX_BUF_FULL;
    }

    if(data && (data_len > 0))
    {
        for(i = 0; i < data_len; i++)
        {
            Common_put_Tx_char(eCOMM_USARTx, data[i]);
        }
        return COMMON_NO_ERROR;
    }else{
        return COMMON_PARAM_INV;
    }
}

uint8_t Common_get_Tx_char(eCommon_USARTx eCOMM_USARTx,uint8_t *error)
{
    sCommon_Ring_Buf *pbuf;
    uint8_t ichar;

    if(eCOMM_USARTx >= COMMON_USARTX_MAX)
    {
        *error = COMMON_INV_CH_ERROR;
        return (NUL);
    }

    pbuf = &CommonRingBuf[eCOMM_USARTx];

    if(pbuf->RingBufTxCtr > 0)
    {
        pbuf->RingBufTxCtr--;
        ichar = *pbuf->RingBufTxOutPtr++;
        if(pbuf->RingBufTxOutPtr == &pbuf->RingBufTx[COMMON_TX_BUF_SIZE])
        {
            pbuf->RingBufTxOutPtr = &pbuf->RingBufTx[0];
        }
        *error = COMMON_NO_ERROR;
        return (ichar);
    }else{
        *error = COMMON_TX_BUF_EMPTY;
        return (NUL);
    }
}

uint8_t Common_TxBuf_is_Empty(eCommon_USARTx eCOMM_USARTx)
{
    sCommon_Ring_Buf *pbuf;
    bool_t is_empty = false;

    if(eCOMM_USARTx >= COMMON_USART_MAX)
    {
        return (0xff);
    }

    pbuf = &CommonRingBuf[eCOMM_USARTx];
    if(pbuf->RingBufTxCtr > 0)
    {
        is_empty = false;
    }else{
        is_empty = true;
    }
    return is_empty;
}

uint8_t Common_TxBuf_is_Full(eCommon_USARTx eCOMM_USARTx)
{
    sCommon_Ring_Buf *pbuf;
    bool_t is_full = false;

    if(eCOMM_USARTx >= COMMON_USART_MAX)
    {
        return (0xff);
    }

    pbuf = &CommonRingBuf[eCOMM_USARTx];
    if(pbuf->RingBufTxCtr < COMMON_TX_BUF_SIZE)
    {
        is_full = false;
    }else{
        is_full = true;
    }
    return is_full;
}

uint8_t Common_get_Rx_char(eCommon_USARTx eCOMM_USARTx,uint8_t *error)
{
    sCommon_Ring_Buf *pbuf;
    uint8_t ichar;

    if(eCOMM_USARTx >= COMMON_USART_MAX)
    {
        *error = COMMON_INV_CH_ERROR;
        return (NUL);
    }

    pbuf = &CommonRingBuf[eCOMM_USARTx];

    if(pbuf->RingBufRxCtr > 0)
    {
        pbuf->RingBufRxCtr--;
        ichar = *pbuf->RingBufRxOutPtr++;
        if(pbuf->RingBufRxOutPtr == &pbuf->RingBufRx[COMMON_RX_BUF_SIZE])
        {
            pbuf->RingBufRxOutPtr = &pbuf->RingBufRx[0];
        }
        *error = COMMON_NO_ERROR;
        return (ichar);
    }else{
        *error = COMMON_TX_BUF_EMPTY;
        return (NUL);
    }
}

void Common_Send_data(eCommon_USARTx eCOMM_USARTx,uint8_t *error)
{
    if(eCOMM_USARTx >= COMMON_USART_MAX)
    {
        *error = COMMON_INV_CH_ERROR;
        return;
    }

    if(USART1->SR & (1 << 7))
    {
        USART1->DR = Common_get_Tx_char(eCOMM_USARTx, error);
    }
}

uint8_t Common_put_Rx_char(eCommon_USARTx eCOMM_USARTx, uint8_t byte)
{
    sCommon_Ring_Buf *pbuf;

    if(eCOMM_USARTx >= COMMON_USART_MAX)
    {
        return COMMON_INV_CH_ERROR;
    }

    pbuf = &CommonRingBuf[eCOMM_USARTx];
    pbuf->RingBufRxCtr++;
    *pbuf->RingBufRxInPtr++ = byte;
    if(pbuf->RingBufRxInPtr == &pbuf->RingBufRx[COMMON_RX_BUF_SIZE])
    {
        pbuf->RingBufRxInPtr = &pbuf->RingBufRx[0];
    }
    return COMMON_NO_ERROR;
}

uint8_t Common_RxBuf_is_Empty(eCommon_USARTx eCOMM_USARTx)
{
    sCommon_Ring_Buf *pbuf;
    bool_t is_empty = false;

    if(eCOMM_USARTx >= COMMON_USART_MAX)
    {
        return (0xff);
    }

    pbuf = &CommonRingBuf[eCOMM_USARTx];
    if((pbuf->RingBufRxCtr) > 0)
    {
        is_empty = false;
    }else{
        is_empty = true;
    }
    return is_empty;
}

uint8_t Common_RxBuf_is_Full(eCommon_USARTx eCOMM_USARTx)
{
    sCommon_Ring_Buf *pbuf;
    bool_t is_full = false;

    if(eCOMM_USARTx >= COMMON_USART_MAX)
    {
        return (0xff);
    }

    pbuf = &CommonRingBuf[eCOMM_USARTx];
    if((pbuf->RingBufRxCtr) < COMMON_TX_BUF_SIZE)
    {
        is_full = false;
    }else{
        is_full = true;
    }
    return is_full;
}

void rt_kprintf(const char *fmt, ...)
{
    va_list args;
    uint32_t length;
    static char rt_log_buf[RT_CONSOLEBUF_SIZE];

    va_start(args, fmt);
    length = vsnprintf(rt_log_buf, sizeof(rt_log_buf) - 1, fmt, args);
    /* 判断字符串是否超过大小范围，超过的话要裁剪 */
    if (length > RT_CONSOLEBUF_SIZE - 1)
        length = RT_CONSOLEBUF_SIZE - 1;
 /* 如果使用设备驱动，通过设备驱动函数将缓冲区的内容输出到控制台 */
#ifdef RT_USING_DEVICE
    if (_console_device == RT_NULL)
    {
        rt_hw_console_output(rt_log_buf);
    }
    else
    {
        rt_uint16_t old_flag = _console_device->open_flag;

        _console_device->open_flag |= RT_DEVICE_FLAG_STREAM;
        rt_device_write(_console_device, 0, rt_log_buf, length);
        _console_device->open_flag = old_flag;
    }
/* 如果不使用设备驱动函数，使用rt_hw_console_output()函数将缓冲区的内容输出到控制台 */
#else
    rt_hw_console_output(rt_log_buf);
#endif
    va_end(args);
}

void rt_hw_console_output( const char *str )
{
	while(*str != '\0')
	{
		Send_Byte(USART1, *str++);
	}
    while ((USART1->SR & (1 << 6)) == 0);
}

