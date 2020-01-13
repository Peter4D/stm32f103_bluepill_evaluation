/**
 * @file Serial.h
 * @author Peter Medvesek (peter.medvesek@gorenje.com)
 * @brief module for serial (uart) communication.
 * @version 0.1
 * @date 2020-01-08 | modified: 2020-01-13
 * 
 * @copyright Copyright (c) 2020 Gorenje d.o.o
 * 
 */

#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>
#include <stddef.h>

#include "ring_buffer.h"

//=======================================================================================
/**
 * @brief set to 1 if used more then 1 hardware serial interface simultaneously
 * @note need to be configured by cubeMX first, to provide hardware initialization first
 */
#define USE_SERIAL_0        1
#define USE_SERIAL_1        0
#define USE_SERIAL_2        0
//=======================================================================================

typedef struct _serial_ctrl_desc_t{
    void                *p_uartHW;   // pointer to HAL uart hardware
    ringBuff_t          *p_xBuff_Tx; // pointer to ring buffer descriptor struct Tx 
    ringBuff_data_t     *p_data_Tx;  // pointer to data buffer Tx
    ringBuff_t          *p_xBuff_Rx; // pointer to ring buffer descriptor struct Tx
    ringBuff_data_t     *p_data_Rx;  // pointer to data buffer Rx
    uint8_t             byteTemp_Rx; // received byte is first saved here and then pushed to buffer
    uint8_t             Rx_active_F; // flag that set if Rx is active or not
    uint8_t             Tx_active_F; // flag that set if Tx is active or not
    uint32_t            last_tm;     // last time that character was received
}serial_ctrl_desc_t;

/**
 * @brief struct of all available methods of this module 
 */
typedef struct _Serial_methods_t{
    // public
    void     (*write)        (serial_ctrl_desc_t *p_ctrl_desc, uint8_t *pSurce, size_t size);
    void     (*print)        (serial_ctrl_desc_t *p_ctrl_desc, const uint8_t * const pStr); // writes until '\0'
    void     (*println)      (serial_ctrl_desc_t *p_ctrl_desc, const uint8_t * const pStr); // writes until '\0'
    void     (*read_enable)  (serial_ctrl_desc_t *p_ctrl_desc);
    uint16_t (*read)         (serial_ctrl_desc_t *p_ctrl_desc, uint8_t *pDest, uint8_t nBytes);
    uint16_t (*readUntil)    (serial_ctrl_desc_t *p_ctrl_desc, uint8_t *pDest, uint8_t nBytes, uint8_t terminate_chr);
    uint16_t (*isData)       (serial_ctrl_desc_t *p_ctrl_desc);
    void     (*flush)        (serial_ctrl_desc_t *p_ctrl_desc);
    uint32_t (*Rx_lastTime)  (serial_ctrl_desc_t *p_ctrl_desc);

}Serial_methods_t;


/**
 * @brief struct that hold user methods for this module
 */
extern Serial_methods_t Serial;

/**
 * @brief serial module descriptors 
 */
extern serial_ctrl_desc_t serial_0;
extern serial_ctrl_desc_t serial_1;
extern serial_ctrl_desc_t serial_2;

/**
 * @brief link serial descriptor to uart HW provided by HAL layer. Initialize internal ring buffers
 * @param p_Serial_ctrl_desc    : pointer to serial HW descriptor
 * @param p_HW_handle           : pointer to HAL hardware structure for particular uart HW
 */
void Serial_init(serial_ctrl_desc_t *p_Serial_ctrl_desc, void *p_HW_handle);

#endif /* SERIAL_H */
