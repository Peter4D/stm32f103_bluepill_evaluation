/**
 * @file Serial.c
 * @author Peter Medvesek (peter.medvesek@gorenje.com)
 * @brief this implemntation use STM HAL framework. project need first be configured 
 * by cubeMX. Setup: serial HW (Clock, pins, speed, ...) 
 * @version 1.0
 * @date 2020-01-08 | modified: 2020-01-13
 * 
 * @copyright Copyright (c) 2020 Gorenje d.o.o
 * 
 */
#include "Serial.h"
/* dependencies */
#include "stm32f1xx_hal.h"
#include "assert_gorenje.h"

//=========================================================
/*Set buffer size for different HW serial channels */
/* serial_0 descriptor */
#define BUFF_0_TX_SIZE          64
#define BUFF_0_RX_SIZE          64

/* serial_1 descriptor */
#define BUFF_1_TX_SIZE          64
#define BUFF_1_RX_SIZE          64

/* serial_2 descriptor */
#define BUFF_2_TX_SIZE          64
#define BUFF_2_RX_SIZE          64
//=========================================================

/* methods declarations */

/**
* @brief print out null terminated string 
* @param p_ctrl_desc   : pointer to serial HW descriptor
* @param pStr          : pointer to null terminated string
*/
void        print       (serial_ctrl_desc_t *p_ctrl_desc, const uint8_t * const pStr);

/**
* @brief print out null terminated string and add new line to the end
* @param p_ctrl_desc   : pointer to serial HW descriptor
* @param pStr          : pointer to null terminated string
*/
void        println     (serial_ctrl_desc_t *p_ctrl_desc, const uint8_t * const pStr);

/**
 * @brief write data to uart (actually to ring buffer)
 * @param p_ctrl_desc   : pointer to serial HW descriptor
 * @param pSurce        : pointer to data source buffer that will get send over uart
 * @param size          : number of bytes that will be send 
 */
void        write       (serial_ctrl_desc_t *p_ctrl_desc, uint8_t *const pSurce, size_t size);

/**
 * @brief enable receive for this HW uart channel
 * @param p_ctrl_desc   : pointer to serial HW descriptor
 */
void        read_enable (serial_ctrl_desc_t *p_ctrl_desc);

/**
 * @brief read data from serial uart internal ring buffer
 * @param p_ctrl_desc   : pointer to serial HW descriptor
 * @param pDest         : pointer to where data will be read to 
 * @param nBytes        : number of bytes that will get read from internal ring buffer
 * @return uint16_t     : actual number that was read from buffer. This could be smaller then nBytes,
 * if buffer get empty first
 */
uint16_t    read        (serial_ctrl_desc_t *p_ctrl_desc, uint8_t *const pDest, uint8_t nBytes);

/**
 * @brief read received uart data from internal buffer until hit a temination character 
 * @param p_ctrl_desc   : pointer to serial HW descriptor
 * @param pDest         : pointer to where data will be read to 
 * @param nBytes        : number of bytes that will be read from internal ring buffer
 * @param terminate_chr : character that is use to stop the read i.e. '\r' -> char for enter key
 * @return uint16_t     : actual number that was read from buffer. This could be smaller then nBytes,
 * if buffer get empty first
 */
uint16_t    readUntil   (serial_ctrl_desc_t *p_ctrl_desc, uint8_t *pDest, uint8_t nBytes, uint8_t terminate_chr);

/**
 * @brief return number of bytes in receive buffer 
 * @param p_ctrl_desc   : pointer to serial HW descriptor
 * @return uint16_t     : number of bytes in Rx buffer
 */
uint16_t    isData      (serial_ctrl_desc_t *p_ctrl_desc);

/**
 * @brief clear receive buffer 
 * @param p_ctrl_desc   : pointer to serial HW descriptor
 */
void        flush       (serial_ctrl_desc_t *p_ctrl_desc);

/**
 * @brief return time in ms from application start when last byte was received from serial bus
 * @param p_ctrl_desc   : pointer to serial HW descriptor
 * @return uint32_t     : last time some character was received over uart
 */
uint32_t    Rx_lastTime (serial_ctrl_desc_t *p_ctrl_desc);

void not_implemented(void);

static HAL_StatusTypeDef HAL_status;

//=========================================================
/* create needed object  */

#if ( USE_SERIAL_0 == 1 )
    ringBuff_t xBuff_0_Tx;
    ringBuff_t xBuff_0_Rx;

    ringBuff_data_t buff_0_Tx[BUFF_0_TX_SIZE]; 
    ringBuff_data_t buff_0_Rx[BUFF_0_RX_SIZE]; 

    /* instance of control block */
    serial_ctrl_desc_t serial_0 = {
        NULL,
        &xBuff_0_Tx,
        &buff_0_Tx,
        &xBuff_0_Rx,
        &buff_0_Rx,
        0,
        0,
        0,
        0
    };
#endif

#if ( USE_SERIAL_1 == 1 )

    ringBuff_t xBuff_1_Tx;
    ringBuff_t xBuff_1_Rx;

    ringBuff_data_t buff_1_Tx[BUFF_1_TX_SIZE]; 
    ringBuff_data_t buff_1_Rx[BUFF_1_RX_SIZE]; 

    serial_ctrl_desc_t serial_1 = {
        NULL,
        &xBuff_1_Tx,
        &buff_1_Tx,
        &xBuff_1_Rx,
        &buff_1_Rx,
        0,
        0,
        0,
        0
    };

#endif

#if ( USE_SERIAL_2 == 1 )

    ringBuff_t xBuff_2_Tx;
    ringBuff_t xBuff_2_Rx;

    ringBuff_data_t buff_2_Tx[BUFF_2_TX_SIZE]; 
    ringBuff_data_t buff_2_Rx[BUFF_2_RX_SIZE]; 
    
    serial_ctrl_desc_t serial_2 = {
        NULL,
        &xBuff_2_Tx,
        &buff_2_Tx,
        &xBuff_2_Rx,
        &buff_2_Rx,
        0,
        0,
        0,
        0
    };

#endif
//=========================================================

//=========================================================
/* set methods for user to access it */
Serial_methods_t Serial = {
    &write,
    &print,
    &println,
    &read_enable,
    &read,
    &readUntil,
    &isData,
    &flush,
    &Rx_lastTime
};
//=========================================================

/* constructor */
void Serial_init(serial_ctrl_desc_t *p_Serial_ctrl_desc, void *p_HW_handle) {

    assert(p_Serial_ctrl_desc != NULL);
    assert(p_HW_handle != NULL);

    RingBuff_init(p_Serial_ctrl_desc->p_xBuff_Tx, p_Serial_ctrl_desc->p_data_Tx, BUFF_0_TX_SIZE);
    RingBuff_init(p_Serial_ctrl_desc->p_xBuff_Rx, p_Serial_ctrl_desc->p_data_Rx, BUFF_0_RX_SIZE);

    p_Serial_ctrl_desc->p_uartHW = (UART_HandleTypeDef*)p_HW_handle;
}


//=========================================================
/* methods implementation */

void print(serial_ctrl_desc_t *p_ctrl_desc, const uint8_t * const pStr){
    /* write to ring buffer and start send if not currently not active */
    uint_fast8_t i = 0;
    static uint_fast8_t byte2send;

    while (pStr[i] != 0x00) // const c-strings are '\0'(0x00) terminated 
    {
        // write to buffer
        RingBuff.push(p_ctrl_desc->p_xBuff_Tx, pStr[i++]);
    }
        
    if (p_ctrl_desc->Tx_active_F == 0)
    {
        // initiate send
        byte2send = RingBuff.get(p_ctrl_desc->p_xBuff_Tx);
        HAL_status = HAL_UART_Transmit_IT(p_ctrl_desc->p_uartHW, &byte2send, 1);
        if (HAL_status != HAL_OK)
        {
            assert(0);
        }
        p_ctrl_desc->Tx_active_F = 1;
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
        
    if (p_ctrl_desc->Tx_active_F == 0)
    {
        // initiate send
        byte2send = RingBuff.get(p_ctrl_desc->p_xBuff_Tx);
        HAL_status = HAL_UART_Transmit_IT(p_ctrl_desc->p_uartHW, &byte2send, 1);
        if (HAL_status != HAL_OK)
        {
            assert(0);
        }
        p_ctrl_desc->Tx_active_F = 1;
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

uint32_t Rx_lastTime(serial_ctrl_desc_t *p_ctrl_desc){
    return p_ctrl_desc->last_tm;
}

void read_enable(serial_ctrl_desc_t *p_ctrl_desc) {
    /* start read */
    if(p_ctrl_desc->Rx_active_F == 0) {
        HAL_UART_Receive_IT(p_ctrl_desc->p_uartHW, &p_ctrl_desc->byteTemp_Rx, 1);
        p_ctrl_desc->Rx_active_F = 1;
    }
}



uint16_t read(serial_ctrl_desc_t *p_ctrl_desc, uint8_t *pDest, uint8_t nBytes) {
    uint8_t i = 0;
    uint8_t byte_cnt = 0;
    
    for (i = 0; i < nBytes; ++i)
    {
        byte_cnt = RingBuff.get_nBytes(p_ctrl_desc->p_xBuff_Rx);	
        if(byte_cnt > 0) {
            pDest[i] = RingBuff.get(p_ctrl_desc->p_xBuff_Rx);
        }else {
            /* buffer empty */
            break;
        }
    }
    return i;
}

uint16_t readUntil(serial_ctrl_desc_t *p_ctrl_desc, uint8_t *pDest, uint8_t nBytes, uint8_t terminate_chr) {
    uint8_t i = 0;
    uint8_t byte_cnt = 0;
    
    for (i = 0; i < nBytes; ++i)
    {
        byte_cnt = RingBuff.get_nBytes(p_ctrl_desc->p_xBuff_Rx);	
        if(byte_cnt > 0) {
            pDest[i] = RingBuff.get(p_ctrl_desc->p_xBuff_Rx);
            if(pDest[i] == terminate_chr) {
                /* replace termination character with 0x00 termination */
                pDest[i] = 0x00;
                break;
            }
        }else {
            /* buffer empty */
            break;
        }
    }
    return i;
}
//=========================================================

/* called from HAL leyer interrupt */
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    /* this callback function could run ring buffer to handle multiple messages */ 
    serial_ctrl_desc_t *p_serial;
    static uint_fast8_t byte2send;

    if(serial_0.p_uartHW == huart) {
        p_serial = &serial_0;
    }
    #if ( USE_SERIAL_1 == 1 )
        else if(serial_1.p_uartHW == huart){
            p_serial = &serial_1;
        }
    #endif
    #if ( USE_SERIAL_2 == 1 )
        else if(serial_2.p_uartHW == huart){
            p_serial = &serial_2;
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
        p_serial->Tx_active_F = 0;
    }
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    serial_ctrl_desc_t *p_serial;

    if(serial_0.p_uartHW == huart) {
        p_serial = &serial_0;
    }
    #if ( USE_SERIAL_1 == 1 )
        else if(serial_1.p_uartHW == huart){
            p_serial = &serial_1;
        }
    #endif
    #if ( USE_SERIAL_2 == 1 )
        else if(serial_2.p_uartHW == huart){
            p_serial = &serial_2;
        }
    #endif

    assert(p_serial != NULL);
    /* save received byte into ringBuffer */
    RingBuff.push(p_serial->p_xBuff_Rx, p_serial->byteTemp_Rx);

    serial_0.last_tm = HAL_GetTick();

    /* reenable Rx */
    HAL_UART_Receive_IT(p_serial->p_uartHW, &p_serial->byteTemp_Rx, 1);
}


void not_implemented(void) {
    /* empty */
    assert(0);
}
