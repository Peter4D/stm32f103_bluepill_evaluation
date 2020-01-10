/**
 * @file Serial.c
 * @author Peter Medvesek (peter.medvesek@gorenje.com)
 * @brief this implemntation use STM HAL framework. project need first be configured 
 * by cubeMX. Setup: serial HW (Clock, pins, speed, ...) 
 * @version 0.1
 * @date 2020-01-08
 * 
 * @copyright Copyright (c) 2020 Gorenje d.o.o
 * 
 */
#include "Serial.h"
/* dependencies */

#include "stm32f1xx_hal.h"
#include "assert_gorenje.h"


#define BUFF_0_TX_SIZE          64
#define BUFF_0_RX_SIZE          64

#define BUFF_1_TX_SIZE          64
#define BUFF_1_RX_SIZE          64

#define BUFF_2_TX_SIZE          64
#define BUFF_2_RX_SIZE          64

/* methods declarations */
void        print   (serial_ctrl_desc_t *p_ctrl_desc, const uint8_t * const pStr);
void        println (serial_ctrl_desc_t *p_ctrl_desc, const uint8_t * const pStr);
void        write   (serial_ctrl_desc_t *p_ctrl_desc, uint8_t *const pSurce, size_t size);

uint16_t    isData  (serial_ctrl_desc_t *p_ctrl_desc);
void        flush   (serial_ctrl_desc_t *p_ctrl_desc);

void not_implemented(void);

static HAL_StatusTypeDef HAL_status;


#if ( USE_SERIAL_0 == 1 )
    ringBuff_t xBuff_0_Tx;
    ringBuff_t xBuff_0_Rx;

    ringBuff_data_t buff_0_Tx[BUFF_0_TX_SIZE]; 
    ringBuff_data_t buff_0_Rx[BUFF_0_RX_SIZE]; 

    /* instance of control block */
    serial_ctrl_desc_t serial_0_desc = {
        NULL,
        &xBuff_0_Tx,
        &buff_0_Tx,
        &xBuff_0_Rx,
        &buff_0_Rx,
        0
    };
#endif

#if ( USE_SERIAL_1 == 1 )
    ringBuff_t xBuff_1_Tx;
    ringBuff_t xBuff_1_Rx;

    ringBuff_data_t buff_1_Tx[BUFF_1_TX_SIZE]; 
    ringBuff_data_t buff_1_Rx[BUFF_1_RX_SIZE]; 

    serial_ctrl_desc_t serial_1_desc = {
        NULL,
        &xBuff_1_Tx,
        &xBuff_1_Rx,
        0
    };
#endif

#if ( USE_SERIAL_2 == 1 )
    ringBuff_t xBuff_2_Tx;
    ringBuff_t xBuff_2_Rx;

    ringBuff_data_t buff_2_Tx[BUFF_2_TX_SIZE]; 
    ringBuff_data_t buff_2_Rx[BUFF_2_RX_SIZE]; 
    
    serial_ctrl_desc_t serial_2_desc = {
        NULL,
        &xBuff_2_Tx,
        &xBuff_2_Rx,
        0
    };
#endif

    // void    (*write)        (serial_ctrl_desc_t *p_ctrl_desc, uint8_t *pSurce, size_t size);
    // void    (*print)        (serial_ctrl_desc_t *p_ctrl_desc, const uint8_t * const pStr); // writes until \0
    // void    (*println)      (serial_ctrl_desc_t *p_ctrl_desc, const uint8_t * const pStr); // writes until \0
    // uint8_t (*read)         (serial_ctrl_desc_t *p_ctrl_desc, uint8_t *pDest, uint8_t nBytes);
    // uint8_t (*isData)       (serial_ctrl_desc_t *p_ctrl_desc);
    // void    (*flush)        (serial_ctrl_desc_t *p_ctrl_desc);

Serial_methods_t Serial = {
    &write,
    &print,
    &println,
    &not_implemented,
    &isData,
    &flush,
};

void Serial_init(serial_ctrl_desc_t *p_Serial_ctrl_desc, void *p_HW_handle) {

    assert(p_Serial_ctrl_desc != NULL);
    assert(p_HW_handle != NULL);

    RingBuff_init(p_Serial_ctrl_desc->p_xBuff_Tx, p_Serial_ctrl_desc->p_data_Tx, BUFF_0_TX_SIZE),

    p_Serial_ctrl_desc->p_uartHW = (UART_HandleTypeDef*)p_HW_handle; // #todo :check if * is needed in casting ?
}


void print(serial_ctrl_desc_t *p_ctrl_desc, const uint8_t * const pStr){
    /* write to ring buffer and start send if not currently not active */
    uint_fast8_t i = 0;
    static uint_fast8_t byte2send;

    while (pStr[i] != 0x00) // const c-strings are '\0'(0x00) terminated 
    {
        // write to buffer
        RingBuff.push(p_ctrl_desc->p_xBuff_Tx, pStr[i++]);
    }
        
    if (p_ctrl_desc->bussy_F == 0)
    {
        // initiate send
        byte2send = RingBuff.get(p_ctrl_desc->p_xBuff_Tx);
        HAL_status = HAL_UART_Transmit_IT(p_ctrl_desc->p_uartHW, &byte2send, 1);
        if (HAL_status != HAL_OK)
        {
            assert(0);
        }
        p_ctrl_desc->bussy_F = 1;
    }
}

void println(serial_ctrl_desc_t *p_ctrl_desc, const uint8_t * const pStr){
    print(p_ctrl_desc, pStr);
    print(p_ctrl_desc, "\n");
}

void write(serial_ctrl_desc_t *p_ctrl_desc, uint8_t *const pSurce, size_t size){
    /* write to ring buffer and start send if not currently not active */
    uint_fast8_t i = 0;
    static uint_fast8_t byte2send;

    while (i < size) // const c-strings are '\0'(0x00) terminated 
    {
        // write to buffer
        RingBuff.push(p_ctrl_desc->p_xBuff_Tx, pSurce[i++]);
    }
        
    if (p_ctrl_desc->bussy_F == 0)
    {
        // initiate send
        byte2send = RingBuff.get(p_ctrl_desc->p_xBuff_Tx);
        HAL_status = HAL_UART_Transmit_IT(p_ctrl_desc->p_uartHW, &byte2send, 1);
        if (HAL_status != HAL_OK)
        {
            assert(0);
        }
        p_ctrl_desc->bussy_F = 1;
    }
}


uint16_t isData (serial_ctrl_desc_t *p_ctrl_desc) {
    uint_fast16_t data_cnt = 0;
    data_cnt = RingBuff.get_nBytes(p_ctrl_desc->p_xBuff_Rx);
    return (data_cnt);
}

void flush(serial_ctrl_desc_t *p_ctrl_desc) {
    RingBuff.flush(p_ctrl_desc->p_xBuff_Rx);
}


void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    /* this callback function could run ring buffer to handle multiple messages */ 
    serial_ctrl_desc_t *p_serial;
    static uint_fast8_t byte2send;

    if(serial_0_desc.p_uartHW == huart) {
        p_serial = &serial_0_desc;
    }
    #if ( USE_SERIAL_1 == 1 )
        else if(serial_1_desc.p_uartHW == huart){
            p_serial = &serial_1_desc;
        }
    #endif
    #if ( USE_SERIAL_2 == 1 )
        else if(serial_2_desc.p_uartHW == huart){
            p_serial = &serial_2_desc;
        }
    #endif

    assert(p_serial != NULL);

    if(RingBuff.get_nBytes(p_serial->p_xBuff_Tx) > 0) {

        byte2send = RingBuff.get(p_serial->p_xBuff_Tx);
        HAL_status = HAL_UART_Transmit_IT(p_serial->p_uartHW, &byte2send, 1);
        if (HAL_status != HAL_OK)
        {
            assert(0);
        }
    }else {
        /* no more data to send */
        p_serial->bussy_F = 0;
    }
}


void not_implemented(void) {
    /* empty */
    assert(0);
}
