#include "bsp_serial.h"
#include "stm32f10x.h"
#include "bsp_systick.h"

void SerialPortInit(unsigned long ulWantedBaud)
{
    USART_InitTypeDef USART_InitStructure;
    USART_ClockInitTypeDef USART_ClockInitStruct;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd( RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE );	

    /* Configure USART1 Rx (PA10) as input floating */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init( GPIOA, &GPIO_InitStructure );
    
    /* Configure USART1 Tx (PA9) as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init( GPIOA, &GPIO_InitStructure );

    USART_InitStructure.USART_BaudRate = ulWantedBaud;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No ;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_ClockInitStruct.USART_Clock = USART_Clock_Disable;
    USART_ClockInitStruct.USART_CPOL = USART_CPOL_Low;
    USART_ClockInitStruct.USART_CPHA = USART_CPHA_2Edge;
    USART_ClockInitStruct.USART_LastBit = USART_LastBit_Disable;
    
    USART_Init( USART1, &USART_InitStructure );
    USART_ClockInit( USART1, &USART_ClockInitStruct);
    USART_Cmd( USART1, ENABLE );
}

void Send_Byte(USART_TypeDef *USARTx, uint16_t byte)
{
    while (0 == (USARTx->SR & (1 << 7)));
    USARTx->DR = (byte & (uint16_t)0x1ff);
}

uint8_t USART_Transmit(USART_TypeDef *USARTx, uint8_t *pTxData, uint16_t Size, uint32_t Timeout)
{
    uint16_t* tmp;
    uint32_t tickstart = 0U;

    tickstart = HAL_GetTick();

    while(Size > 0)
    {
        Size--;
        while(USART_GetITStatus(USARTx, USART_IT_TXE) == RESET)
        {
            if((HAL_GetTick - tickstart) > Timeout)
                return 1;
        }
        USART_SendData(USARTx, *pTxData++ & (uint8_t)0xFF);
    }
    return 0;
}

uint8_t Receive_Byte(USART_TypeDef *USARTx)
{
    uint8_t byte;

    while (0 == (USARTx->SR & (1 << 5)));
    byte = (uint8_t)USARTx->DR;
    
    return byte;
}

int fputc( int ch, FILE *f)
{
    USART_TypeDef *USARTx = USART1;
    while (0 == ((USARTx->SR) & (1 << 7)));
    USARTx->DR = (ch & (uint16_t)0x01ff);

    return ch;
}

int fgetc(FILE* f)
{
    USART_TypeDef* USARTx = USART1;
    while (0 == (USARTx->SR & (1<<5)));
    
    return (int)(USARTx->DR & 0xFF);
}
