#include "spi.h"



void spi_Init(void)
{
    SPI_InitTypeDef SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    /* ʹ��SPI������ص�ʱ�� */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1,
                               ENABLE);

    /* ����SPI�� CS���ţ���ͨIO���� */
    // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    // GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    // GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    // GPIO_Init(GPIOA, &GPIO_InitStructure);

    /* ����SPI�� SCK MOSI MISO����*/
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    // GPIO_SetBits(GPIOA, GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);

    /* ֹͣ�ź� FLASH: CS���Ÿߵ�ƽ*/
    // GPIO_SetBits(GPIOA, GPIO_Pin_4);

    /* SPI ģʽ���� */
    // FLASHоƬ ֧��SPIģʽ0��ģʽ3���ݴ�����CPOL CPHA
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Hard;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStructure);

    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = SPI1_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
    /* ʹ�� SPI  */
    SPI_Cmd(SPI1, ENABLE);
    SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_RXNE, ENABLE);
}

volatile uint8_t recv[256] = {0};
volatile uint8_t flag = 0;

void SPI1_IRQHandler(void)
{
    static uint32_t i = 0;
    if(SPI_I2S_GetITStatus(SPI1, SPI_I2S_IT_RXNE) == SET) {
        if(i >= 256) {
            i = 0;
            flag = 1;
        }
        recv[i++] = SPI_I2S_ReceiveData(SPI1);
        SPI_I2S_ClearITPendingBit(SPI1, SPI_I2S_IT_RXNE);
    }
}

void recv_display(void)
{
    uint32_t i, j;
    if(flag) {
        flag = 0;
        for(i = 0; i < 256; i++) {
            printf("%d ", recv[i]);
            SPI_I2S_SendData(SPI1, recv[i]);
        }
        printf("\r\n\r\n");
    }
}