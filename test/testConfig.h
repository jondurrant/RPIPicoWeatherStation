/*
 * testConfig.h
 *
 *  Created on: 5 Sept 2024
 *      Author: jondurrant
 */

#ifndef TEST_TESTCONFIG_H_
#define TEST_TESTCONFIG_H_

#include "FreeRTOS.h"
#include "task.h"

#define I2CCHAN i2c1
#define SDA1_PAD 18
#define SCL1_PAD 19

#define SDA0_PAD 20
#define SCL0_PAD 21
#define RTC_VCC 22
#define RTC_BAT 27

#define SWT_PAD 6
#define LED_PAD 7
#define RED_PAD 7
#define GRN_PAD 10
#define BLU_PAD 11

#define WAKE_PAD 26

#define ANEM_PAD 13
#define DIRP_PAD 	   14
#define DIRA_PAD    28

#define SNR_CTR 12

#define RAIN 15

#define TASK_PRIORITY     ( tskIDLE_PRIORITY + 1UL )

#endif /* TEST_TESTCONFIG_H_ */