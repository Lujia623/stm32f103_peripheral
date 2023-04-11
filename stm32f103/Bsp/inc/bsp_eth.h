/**
  ******************************************************************************
  * @file    bsp_eth.h
  * @author  Rougga
  * @version V0.1
  * @date    22-March-2023
  * @brief   This file describes w5500 initialization and lwip stack migration information.
  ******************************************************************************/
#ifndef BSP_ETH_H
#define BSP_ETH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"
#include "w5500.h"
//#include "lwip/err.h"
//#include "lwip/netif.h"

#define W5500_RESET_PORT              GPIOE
#define W5500_RESET_PIN               GPIO_Pin_0
#define W5500_INT_PORT                GPIOE
#define W5500_INT_PIN                 GPIO_Pin_1
#define W5500_SPI_CS_PORT             GPIOE
#define W5500_SPI_CS_PIN              GPIO_Pin_2
#define W5500_SPI_CLK_PORT
#define W5500_SPI_CLK_PIN
#define W5500_SPI_MOSI_PORT
#define W5500_SPI_MOSI_PIN
#define W5500_SPI_MISO_PORT
#define W5500_SPI_MISO_PIN

uint8_t W5500_lwip_init(void);
void  wizchip_cs_select(void);
void wizchip_cs_deselect(void);
void wizchip_spi_writebyte(uint8_t wb);


#ifdef __cplusplus
}
#endif
#endif /* BSP_ETH_H */

