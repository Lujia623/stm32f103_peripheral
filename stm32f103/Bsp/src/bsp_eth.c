#include "bsp_eth.h"
#include "bsp_systick.h"
#include "bsp_serial.h"
#include "bsp_usart.h"
#include <stdlib.h>

static SPI_TypeDef *SPIx = NULL;

static void W5500_config(void)
{
    SPI_InitTypeDef SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOA | RCC_APB2Periph_SPI1 | RCC_APB2Periph_AFIO,
                               ENABLE);
    GPIO_InitStructure.GPIO_Pin = W5500_SPI_CS_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(W5500_SPI_CS_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = W5500_RESET_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(W5500_RESET_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = W5500_INT_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(W5500_INT_PORT, &GPIO_InitStructure);
    GPIO_SetBits(GPIOA, GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);
    GPIO_SetBits(W5500_SPI_CS_PORT, W5500_SPI_CS_PIN);

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStructure);

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource1);
    EXTI_InitStruct.EXTI_Line = EXTI_Line1;
    EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStruct.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStruct);

    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel = EXTI1_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);

    SPI_Cmd(SPI1, ENABLE);
    // SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_RXNE, ENABLE);
}

void EXTI1_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line1) != RESET)
    {
        printf("w5500 INT\r\n");
        EXTI_ClearITPendingBit(EXTI_Line1);
    }
}

static void W5500_reset(void)
{
    GPIO_ResetBits(W5500_RESET_PORT, W5500_RESET_PIN);
    Delay(5);
    GPIO_SetBits(W5500_RESET_PORT, W5500_RESET_PIN);
    Delay(10);
}

uint8_t W5500_lwip_init(void)
{
    uint8_t reg = 0x00;

    W5500_config();
    W5500_reset();

    printf("init seccuss\r\n");
    printf("VERSIONR:%0X\r\n", VERSIONR);
    reg = getVERSIONR();
    printf("reg:%0x\r\n", reg);
    if (reg != 0x04) {
        printf("get version error!!!\r\n");
        return -1;
    }
    printf("reg:%0x\r\n", reg);
    setMR(MR_RST);
    uint8_t mr = getMR();
    printf("mr:%0x\r\n", mr);
    setSn_MR(0, Sn_MR_MACRAW | Sn_MR_MIP6B | Sn_MR_MMB);
    setSn_RXBUF_SIZE(0, 16);
    printf("getSn_RXBUF_SIZE:%0x\r\n", getSn_RXBUF_SIZE(0));
    setSn_TXBUF_SIZE(0, 16);
    printf("getSn_TXBUF_SIZE:%0x\r\n", getSn_TXBUF_SIZE(0));
    setINTLEVEL(1);
    setSIMR(1);
    setPHYCFGR(0x58);
    setPHYCFGR(0xD8);
    setSn_IMR(0, (Sn_IR_RECV));
    setSn_CR(0, Sn_CR_OPEN);
    setSn_IR(0, 0x1F);
    setSIR(0);
    // setSn_CR(0, Sn_CR_RECV);

    return 0;
}

static uint8_t spixSendRecvByte(uint8_t data)
{
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
    SPI_I2S_SendData(SPI1, data);
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
    return SPI_I2S_ReceiveData(SPI1);
    
}

// static uint8_t spixRecvByte(void)
// {
//     while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
//     return SPI_I2S_ReceiveData(SPI1);
// }

void wizchip_cris_enter(void)
{
}

void wizchip_cris_exit(void)
{
}

void  wizchip_cs_select(void)
{
    GPIO_WriteBit(W5500_SPI_CS_PORT, W5500_SPI_CS_PIN, Bit_RESET);
}

void wizchip_cs_deselect(void)
{
    GPIO_WriteBit(W5500_SPI_CS_PORT, W5500_SPI_CS_PIN, Bit_SET);
}

void wizchip_spi_writebyte(uint8_t wb)
{
    spixSendRecvByte(wb);
}

uint8_t wizchip_spi_readbyte(void)
{
    return spixSendRecvByte(0x00);
}
