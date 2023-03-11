#ifndef __SPI_H_
#define __SPI_H_

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_spi.h"
#include "bsp_serial.h"


void spi_Init(void);
void recv_display(void);
#endif /*__SPI_H_*/