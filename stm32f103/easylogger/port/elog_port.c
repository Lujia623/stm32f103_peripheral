/*
 * This file is part of the EasyLogger Library.
 *
 * Copyright (c) 2015, Armink, <armink.ztl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Function: Portable interface for each platform.
 * Created on: 2015-04-28
 */
 
#include <elog.h>
#include "bsp_systick.h"
#include "main.h"

#define TRANSMIT_TIMEOUT            2000U


static SemaphoreHandle_t xMutexElog = NULL;
/**
 * EasyLogger port initialize
 *
 * @return result
 */
ElogErrCode elog_port_init(void) {
    ElogErrCode result = ELOG_NO_ERR;

    /* add your code here */
    if(xMutexElog == NULL)
    {
        xMutexElog = xSemaphoreCreateMutex();
    }
    
    return result;
}

/**
 * EasyLogger port deinitialize
 *
 */
void elog_port_deinit(void) {

    /* add your code here */

}

/**
 * output log port interface
 *
 * @param log output of log
 * @param size log size
 */
void elog_port_output(const char *log, size_t size) {
    
    /* add your code here */
    USART_Transmit(USART1, (uint8_t *)log, size, TRANSMIT_TIMEOUT);
}

/**
 * output lock
 */
void elog_port_output_lock(void) {
    
    /* add your code here */
    if(xMutexElog == NULL) {
        xMutexElog = xSemaphoreCreateMutex();
    }

    if((taskSCHEDULER_NOT_STARTED != xTaskGetSchedulerState()) && (xPortIsInsideInterrupt() == pdFALSE))
        xSemaphoreTake(xMutexElog, portMAX_DELAY);
}

/**
 * output unlock
 */
void elog_port_output_unlock(void) {
    
    /* add your code here */
    if(xMutexElog == NULL) {
        xMutexElog = xSemaphoreCreateMutex();
    }
    if((taskSCHEDULER_NOT_STARTED != xTaskGetSchedulerState()) && (xPortIsInsideInterrupt() == pdFALSE))
        xSemaphoreGive(xMutexElog);
}

/**
 * get current time interface
 *
 * @return current time
 */
const char *elog_port_get_time(void) {
    
    /* add your code here */
    static char cur_system_time[16] = "";
    if (taskSCHEDULER_NOT_STARTED == xTaskGetSchedulerState())
        snprintf(cur_system_time, 16, "%lu", HAL_GetTick());
    else
        snprintf(cur_system_time, 16, "%lu", xTaskGetTickCount());
    return cur_system_time;
    
}

/**
 * get current process name interface
 *
 * @return current process name
 */
const char *elog_port_get_p_info(void) {
    
    /* add your code here */
    return "";
}

/**
 * get current thread name interface
 *
 * @return current thread name
 */
const char *elog_port_get_t_info(void) {
    
    /* add your code here */
    return "";
}