#include "Serial_test.h"
#include "Serial.h"
/* HAL dependencies */
#include "stm32f1xx_hal.h"
#include "usart.h"

/**
 * @brief task execution periods in ms 
 */
#define TASK_1_PER      1000
#define TASK_2_PER      10


static void Test_task_upTime(void);
static void Test_task_reflectMsg(void);

void serial_test_init(void){
    Serial_init(&serial_0, &huart1);
    Serial.read_enable(&serial_0);
}

void serial_test_exe(void) {
    Test_task_upTime();
    Test_task_reflectMsg();
}


static void Test_task_upTime(void) {
    static uint32_t task_1_lastTick = 0;
    static uint32_t upCnt = 0;
    static uint8_t serial_msg[30];
    static uint8_t num_str[10];
    static uint8_t serial_msg_len;

    if( (HAL_GetTick() - task_1_lastTick) > TASK_1_PER) {
        upCnt++;
        strcpy(serial_msg, "upTime in seconds: ");
        num2str(upCnt, num_str);
        strcat(serial_msg, num_str);
        strcat(serial_msg, "\n\r");
        
        serial_msg_len = strlen(serial_msg);
        Serial.write(&serial_0, serial_msg, serial_msg_len);
        
        task_1_lastTick = HAL_GetTick();
    } 
}

#define SER_RX_BUFF_SIZE    50
static void Test_task_reflectMsg(void) {
    static uint32_t task_2_lastTick = 0;
    static uint8_t serRx_buff[SER_RX_BUFF_SIZE];
    static uint16_t read_ch_cnt = 0;

    if( (HAL_GetTick() - task_2_lastTick) > TASK_2_PER) {
        uint8_t serial_Rx_size = 0;

        serial_Rx_size = Serial.isData(&serial_0); 
        if( serial_Rx_size > 0 && (HAL_GetTick() - Serial.Rx_lastTime(&serial_0) > 5) ) {
            /* read all */
            //Serial.read(&serial_0, serRx_buff, serial_Rx_size);
            serial_Rx_size = Serial.readUntil(&serial_0, serRx_buff, SER_RX_BUFF_SIZE, '\r');

            Serial.write(&serial_0, serRx_buff, serial_Rx_size);
            Serial.print(&serial_0, "\r\n");
        }

        task_2_lastTick = HAL_GetTick();
    }
}
