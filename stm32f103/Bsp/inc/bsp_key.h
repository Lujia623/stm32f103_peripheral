#ifndef __BEP_KEY_H__
#define __BEP_KEY_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "stm32f10x.h"

typedef enum
{
    KEY_OFF,
    KEY_ON,
} KEY_STATE;



void key_init(void);
KEY_STATE Scan_key(void);

#ifdef __cplusplus
}
#endif

#endif /*__BEP_KEY_H_*/
