/*
 * Copyright (C) 2023 Rougga
 */
#include "driver.h"

static slist_t device_list;

#define LIST_LOCK()
#define LIST_UNLOCK()

static device_t *device_find(const char *name, int id);

static char *dev_name_parse(const char *name, int *id)
{
    char *n = strdup(name);

    if (n == NULL)
        return NULL;

    int len = strlen(n);

    for (int i = 0; i < len; i++) {
        if (n[i] >= '0' && n[i] <= '9') {
            *id = atoi(n + i);
            n[i] = '\0';
            break;
        }
    }

    return n;
}

/**
 * @brief assign a unique ID
 * 
 * @return int device id
 */
static int alloc_device_id(void)
{
    uint32_t index = rand() & 0xFFFF;

    ///TODO:list lock
    while (1) {
        index++;

        int found = 0;
        device_t *node;
        
        slist_for_each_entry(&device_list, node, device_t, node) {
            driver_t *drv = (driver_t *)node->drv;
            ///TODO:check drv if NULL
            if(drv->device_id == (index & 0xFFFF)) {
                found = 1;
                break;
            }
        }

        if(found == 0)
            break;
    }
    ///TODO:list unlock

    return index & 0xFFFF;
}

device_t *device_new(driver_t *drv, int size, int id)
{
    device_t *dev = (device_t*)malloc(size);

    if (dev) {
        dev->id = id;
        dev->drv = drv;

        // if (aos_mutex_new(&dev->mutex) != 0) {
        //     aos_free(dev);
        //     return NULL;
        // }
    }

    return dev;
}

int driver_register(driver_t *drv, void *config, int idx)
{
    ///TODO:check drv,drv->init

    if(device_find(drv->name, idx) != NULL)
        return -1;
    
    device_t *dev = drv->init(drv, config, idx);

    if(dev) {
        dev->id = idx;
        ((driver_t *)(dev->drv))->device_id = alloc_device_id();
        ///:TODO:device lock
        slist_add_tail(&dev->node, &device_list);
        ///:TODO:device unlock
        return 0;
    }

    return -1;
}

int driver_unregister(const char *name)
{
    int id = 0;

    char *n = dev_name_parse(name, &id);
    if (n == NULL)
        return -12;

    device_t *dev = device_find(n, id);
    free(n);

    if (dev) {
        driver_t *drv = (driver_t *)dev->drv;
        ///TODO:check drv if NULL
        if (drv->uninit)
            drv->uninit(dev);

        LIST_LOCK();
        slist_del(&dev->node, &device_list);
        LIST_UNLOCK();

        return 0;
    }

    return -1;
}

/**
 * @brief find device
 * 
 * @param[in] name device name
 * @param[in] id device id
 * @return device_t* null ont find
 */
static device_t *device_find(const char *name, int id)
{
    device_t *node = NULL;

    LIST_LOCK();
    slist_for_each_entry(&device_list, node, device_t, node) {
        driver_t *drv = (driver_t *)node->drv;
        ///TODO:check dir if NULL
        if(strcmp(drv->name, name) == 0 && node->id == id)
            break;
    }
    LIST_UNLOCK();

    return node;
}

int device_valid(device_t *dev, const char *name)
{
    if(dev && dev->drv) {
        const char *n = NULL;
        driver_t *drv = (driver_t*)dev->drv;
        n = drv->name;
        if (n)
            return strcmp(n, name);
    }

    return -1;
}

device_t *device_open(const char *name)
{
    int id = 0;

    char *n = dev_name_parse(name, &id);
    if(n != NULL) {
        device_t *dev = device_open_id(n, id);
        free(n);
        return dev;
    }

    return NULL;
}

device_t *device_open_id(const char *name, int id)
{
    device_t *dev = device_find(name, id);

    if(dev) {
        ///TODO:device lock
        if(dev->ref == 0) {
            if(DRIVER(dev)->open(dev) != 0) {
                ///TODO:device unlock
            }
        }
        dev->ref++;
        ///TODO:device unlock
    }
    return dev;
}

int device_close(device_t *dev)
{
    int ret = -9;

    if(dev) {
        ///TODO:device lock
        if(dev->ref > 0) {
            dev->ref--;
            if(dev->ref == 0) {
                ret = DRIVER(dev)->close(dev);
            }
        }
        ///TODO:device unlock
    }

    return ret;
}

void device_free(device_t *dev)
{
    ///TODO:mutex free
    free(dev);
}

int device_tags(device_t *dev)
{
    ///TODO:check dev if NULL
    driver_t *drv = DRIVER(dev);
    ///TODO:check drv if NULL
    return drv->device_id;
}

