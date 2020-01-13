/**
 * @file Serial_test.h
 * @author Peter Medvesek (peter.medvesek@gorenje.com)
 * @brief  Every 1s send through serial port (HAL_HW: huart1) send message with up counter value
 * Every message send from i.e. PC is reflected back
 * @version 1.0
 * @date 2020-01-13
 * 
 * @copyright Copyright (c) 2020 Gorenje d.o.o
 * 
 */
#ifndef SERIAL_TEST_H
#define SERIAL_TEST_H

/**
 * @brief Initialize Serial module
 */
void serial_test_init(void);

/**
 * @brief execute the test. Timming is handled in this test, so user only need to call in main() -> while(1)
 */
void serial_test_exe(void);


#endif /* SERIAL_TEST_H */
