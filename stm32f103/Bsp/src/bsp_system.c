#include "bsp_system.h"
#include <stdio.h>
#include <stdint.h>

void assert_failed(char *file, uint32_t line)
{
    printf("Wrong parameters value: file %s on line %d\r\n", file, line);
    while (1)
        ;
}
