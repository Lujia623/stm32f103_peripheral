/**
  ******************************************************************************
  * @file    driver.h
  * @author  Rougga
  * @version V0.1
  * @date    11-March-2023
  * @brief   This file contains all the functions prototypes for the devices firmware 
  *          library.
  ******************************************************************************/
#ifndef DRIVER_H
#define DRIVER_H

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "list.h"
#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _driver_obj driver_t;
struct _driver_obj {
    slist_t     node;
    /* driver name */
    char        *name;
    int16_t     device_id;
    device_t * (*init)(driver_t *drv, void *config, int id);
    void    (*uninit)(device_t *dev);
    int     (*open)(device_t *dev);
    int     (*close)(device_t *dev);
};

#define DRIVER(dev)  ((driver_t*)(dev->drv))

/**
 * @brief             register device object into device driver framework.
 * 
 * @param[in]         drv     device driver object
 * @param[in]         config  device number
 * @param[in]         idx     device number
 * @return int -1 error, 0 success.
 */
int driver_register(driver_t *drv, void *config, int idx);

/**
 * @brief         remove device object from device driver framework.
 * 
 * @param[in]     name device name
 * @return int    -1 error, 0 success
 */
int driver_unregister(const char *name);

/**
 * @brief         get device object by device name
 * 
 * @param[in]     name device name
 * @param[in]     id device index
 * @return        device_t* null on error, device object pointer on success
 */
device_t *device_find(const char *name, int id);

/**
 * @brief         allocate new node for new device driver
 * 
 * @param[in]     drv device driver object
 * @param[in]     size device driver object size
 * @param[in]     id device node index
 * @return        device_t* null on error, device object pointer on success
 */
device_t *device_new(driver_t *drv, int size, int id);

/**
 * @brief         free a device node
 * 
 * @param[in]     dev device driver object
 */
void device_free(device_t *dev);

/**
 * @brief         lock device to prevent to access
 * 
 * @param[in]     dev device driver object
 * @return        int 0 success
 */
int device_lock(device_t *dev);

/**
 * @brief         unlock device to allow to access 
 * 
 * @param[in]     dev device driver object
 * @return        int 0 success
 */
int device_unlock(device_t *dev);

/**
 * @brief          valid a device 's name
 * 
 * @param[in]      dev device driver object
 * @param[in]      name device name
 * @return         int =0 success other is error
 */
int device_valid(device_t *dev, const char *name);

/**
 * @brief          get a device 's id
 * 
 * @param[in]      dev device driver object
 * @return         int device 's id
 */
int device_tags(device_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* DRIVER_H */

