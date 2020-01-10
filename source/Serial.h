/**
 * @file Serial.h
 * @author Peter Medvesek (peter.medvesek@gorenje.com)
 * @brief module for serial (uart) communication.
 * @version 0.1
 * @date 2020-01-08
 * 
 * @copyright Copyright (c) 2020 Gorenje d.o.o
 * 
 */

#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>
#include <stddef.h>

#include "ring_buffer.h"


#define USE_SERIAL_0        1
#define USE_SERIAL_1        0
#define USE_SERIAL_2        0

typedef struct _serial_ctrl_desc_t{
    void                *p_uartHW;
    ringBuff_t          *p_xBuff_Tx;
    ringBuff_data_t     *p_data_Tx;
    ringBuff_t          *p_xBuff_Rx;
    ringBuff_data_t     *p_data_Rx;
    uint8_t             byteTemp_Rx;
    uint8_t             Rx_active_F;
    uint8_t             bussy_F;
}serial_ctrl_desc_t;

typedef struct _Serial_methods_t{
    // public
    void     (*write)        (serial_ctrl_desc_t *p_ctrl_desc, uint8_t *pSurce, size_t size);
    void     (*print)        (serial_ctrl_desc_t *p_ctrl_desc, const uint8_t * const pStr); // writes until \0
    void     (*println)      (serial_ctrl_desc_t *p_ctrl_desc, const uint8_t * const pStr); // writes until \0
    void     (*read_enable)  (serial_ctrl_desc_t *p_ctrl_desc);
    uint16_t (*read)         (serial_ctrl_desc_t *p_ctrl_desc, uint8_t *pDest, uint8_t nBytes);
    uint16_t (*readUntil)    (serial_ctrl_desc_t *p_ctrl_desc, uint8_t *pDest, uint8_t nBytes, uint8_t terminate_chr);
    uint16_t (*isData)       (serial_ctrl_desc_t *p_ctrl_desc);
    void     (*flush)        (serial_ctrl_desc_t *p_ctrl_desc);

}Serial_methods_t;


// handle-methods
extern Serial_methods_t Serial;

/* is used more then this: add more */
extern serial_ctrl_desc_t serial_0_desc;
extern serial_ctrl_desc_t serial_1_desc;
extern serial_ctrl_desc_t serial_2_desc;

// constructor
void Serial_init(serial_ctrl_desc_t *p_Serial_ctrl_desc, void *p_HW_handle);

#endif /* SERIAL_H */
