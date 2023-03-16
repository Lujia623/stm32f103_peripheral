/**
  ******************************************************************************
  * @file    device.h
  * @author  Rougga
  * @version V0.1
  * @date    11-March-2023
  * @brief   This file contains all the functions prototypes for the devices firmware 
  *          library.
  ******************************************************************************/
#ifndef __DEVICE_H_
#define __DEVICE_H_

#include <stdint.h>
#include <string.h>
#include "list.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _dev_obj {
  slist_t     node;
  /* device id */
  uint8_t     id;
  /* device ref */
  uint16_t    ref;
  /* mutex */
  //mutex
  /* drive point */
  void        *drv;
  /*  Private data point*/
  void        *priv;
  /* init config point */
  void        *config;
} device_t;


/**
 * @brief           open one device
 * 
 * @param[in]       name device name
 * @return          device_t* device_t* null on error, device object pointer on success
 */
device_t *device_open(const char *name);

/**
 * @brief         open one device
 * 
 * @param[in]     name device name
 * @param[in]     id device index to be register
 * @return        device_t* null on error, device object pointer on success
 */
device_t *device_open_id(const char *name, int id);

/**
 * @brief          close one device
 * 
 * @param[in]      dev device name
 * @return         int <0 error, >0 return device close error
 */
int device_close(device_t *dev);

#ifdef __cplusplus
}
#endif
#endif /*__DEVICE_DRIVER_H_*/

