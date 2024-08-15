/*
 * DeepSleepRTOS.h
 *
 *  Created on: 16 Jul 2024
 *      Author: jondurrant
 */

#ifndef SRC_DEEPSLEEPRTOS_H_
#define SRC_DEEPSLEEPRTOS_H_

#include "pico/stdlib.h"
#include "DS3231.hpp"
#include "DormantNotification.h"
#include <list>
#include "hardware/clocks.h"

class DeepSleepRTOS {
public:
	virtual ~DeepSleepRTOS();

	/***
	 * Set the RTC
	 * If no RTC will just ignore RTC comms
	 * @param rtc - pointer to the RTC object. Can be NULL
	 */
	void setRTC(DS3231 *rtc);


	/***
	 * Own GPIO Callbacks, meaning any existing GPIO callback on the
	 * core will no longer function.
	 * If using local RTC to wake core this can be disabled
	 * @param on
	 */
	void setOwnGPIOCallbacks(bool on=true);

	/***
	 * Sleep until pad pulled to ground
	 * @param wakePad - GPIO Pad for wake. >28 GPIO wake is not enabled
	 */
	void sleep(uint8_t wakePad = 0xFF);

	/***
	 * Sleep for number of minutes and wake by GPIO pad
	 * If no RTC then it will just do sleep(wakePad)
	 * @param minutes - Minutes to sleep for (<=60)
	 * @param wakePad - GPIO Pad for wake. >28 GPIO wake is not enabled
	 */
	void sleep(uint minutes, uint8_t wakePad=0xFF);

	/***
	 * Sleep for a number of minutes.
	 * Assume woken by internal RTC
	 * @param minutes <=60
	 */
	void sleepMin(uint minutes);


	/***
	 * Get the Deep Sleep control object
	 * @return Deep Sleep object
	 */
	static DeepSleepRTOS * singleton();

	/***
	 * Add observer for sleep and wakeup
	 * @param obs
	 */
	void addObserver(DormantNotification *obs);

	/***
	 * Delete observer for sleep and wakeup
	 * @param obs
	 */
	void delObserver(DormantNotification *obs);


	/***
	 * Enable PWM to function during Deep Sleep
	 */
	void enablePWM();

	/***
	 * Enable Pico RTC to function during Deep Sleep
	 */
	void enableRTC();

	/***
	 * Untested. Enable JTAG and therefore SWD during deepsleep?
	 */
	void enableJTAG();

	/***
	 * UnTested
	 * Enable Uart0 receive  during deep sleep
	 */
	void enableUart0();

	/***
	 * UnTested
	 * Enable Uart1 receive  during deep sleep
	 */
	void enableUart1();

	/***
	 * UnTested
	 * Enable Timers  during deep sleep
	 */
	void enableTimer();

	/***
	 * UnTested
	 * Enable USB receive  during deep sleep
	 */
	void enableUSB();

	/***
	 * UnTested
	 * Enable PIO0   during deep sleep
	 */
	void enablePIO0();

	/***
	 * UnTested
	 * Enable PIO1   during deep sleep
	 */
	void enablePIO1();

	/***
	 * UnTested
	 * Enable DMA   during deep sleep
	 */
	void enableDMA();


private:
	static DeepSleepRTOS * pSingleton ;
	DeepSleepRTOS();

	static void rtcCB(void);
	static void gpio_callback(uint gpio, uint32_t events);

	void recover();

	/***
	 * Reset the clocks
	 * @param scb_orig
	 * @param clock0_orig
	 * @param clock1_orig
	 */
	void recover_from_sleep(
			uint scb_orig,
			uint clock0_orig,
			uint clock1_orig);

	/***
	 * Store the clocks
	 */
	void storeClocks();

	void sleep_until_interupt( ) ;

	/***
	 * Notify observers of going dormant
	 * @param minutes
	 * €
	 */
	void notifyObservers(uint minutes, bool wake=false);

	std::list<DormantNotification *> xObservers;

    volatile bool xOwnGPIOCallbacks = true;
	DS3231 *pRTC = NULL;
	volatile uint scb_orig;
	volatile uint clock0_orig;
	volatile uint clock1_orig;
	volatile io_rw_32 xClocks = 0;
};

#endif /* SRC_DEEPSLEEPRTOS_H_ */
