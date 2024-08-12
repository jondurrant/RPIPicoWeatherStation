#include "pico/sleep.h"
#include "pico/stdlib.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "hardware/clocks.h"
#include "hardware/rosc.h"
#include "hardware/structs/scb.h"
#include "hardware/sync.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"

#define FLASH_LED 7
#define RAIN_PAD 15
#define WAKE_PAD 26


static bool awake;
static bool extWake = false;


void gpio_callback(uint gpio, uint32_t events) {
    extWake = true;
    awake = true;
}

void setupWake(){
	gpio_init(WAKE_PAD);
	gpio_pull_up(WAKE_PAD);
	gpio_set_dir(WAKE_PAD, GPIO_IN);
	gpio_set_irq_enabled_with_callback (
		  WAKE_PAD,
		   GPIO_IRQ_EDGE_FALL,
		   true,
		   &gpio_callback
		   );
}

uint8_t setupRain(){
	gpio_init(RAIN_PAD);
	gpio_pull_up(RAIN_PAD);
	gpio_set_dir(RAIN_PAD, GPIO_IN);

	if (pwm_gpio_to_channel(RAIN_PAD) != PWM_CHAN_B){
		printf("ERROR - GPIO Must be PWM Channel B\n");
	}
   uint slice_num = pwm_gpio_to_slice_num(RAIN_PAD);

   // Count once for every 1 cycles the PWM B input is HIGH
   pwm_config cfg = pwm_get_default_config();
   pwm_config_set_clkdiv_mode(&cfg, PWM_DIV_B_FALLING);
   pwm_config_set_clkdiv(&cfg, 1);
   pwm_init(slice_num, &cfg, false);
   gpio_set_function(RAIN_PAD, GPIO_FUNC_PWM);
   pwm_set_enabled(slice_num, true);
   return slice_num;
}



static void sleep_callback(void) {
    printf("RTC woke us up\n");
    uart_default_tx_wait_blocking();
    awake = true;
    return;
}

void local_sleep_goto_sleep_until(datetime_t *t, rtc_callback_t callback) {
    // We should have already called the sleep_run_from_dormant_source function
    assert(dormant_source_valid(_dormant_source));

    // Turn off all clocks when in sleep mode except for RTC
    clocks_hw->sleep_en0 = CLOCKS_SLEEP_EN0_CLK_RTC_RTC_BITS
    		| CLOCKS_SLEEP_EN0_CLK_SYS_PWM_BITS;
    clocks_hw->sleep_en1 = 0x0;

   rtc_set_alarm(t, callback);

    uint save = scb_hw->scr;
    // Enable deep sleep at the proc
    scb_hw->scr = save | M0PLUS_SCR_SLEEPDEEP_BITS;

    // Go to sleep
    __wfi();
}

static void rtc_sleep(int8_t minute_to_sleep_to, int8_t second_to_sleep_to) {

    datetime_t t_alarm = {
            .year  = 2020,
            .month = 06,
            .day   = 05,
            .dotw  = 5, // 0 is Sunday, so 5 is Friday
            .hour  = 15,
            .min   = minute_to_sleep_to,
            .sec   = second_to_sleep_to
    };

    printf("Going to sleep.......\n");
    uart_default_tx_wait_blocking();

    local_sleep_goto_sleep_until(&t_alarm, &sleep_callback);
}


void recover_from_sleep(uint scb_orig, uint clock0_orig, uint clock1_orig){

    //Re-enable ring Oscillator control
    rosc_write(&rosc_hw->ctrl, ROSC_CTRL_ENABLE_BITS);

    //reset procs back to default
    scb_hw->scr = scb_orig;
    clocks_hw->sleep_en0 = clock0_orig;
    clocks_hw->sleep_en1 = clock1_orig;

    //reset clocks
    clocks_init();
    stdio_init_all();

    return;
}

void measure_freqs(void) {
    uint f_pll_sys = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_PLL_SYS_CLKSRC_PRIMARY);
    uint f_pll_usb = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_PLL_USB_CLKSRC_PRIMARY);
    uint f_rosc = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_ROSC_CLKSRC);
    uint f_clk_sys = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_SYS);
    uint f_clk_peri = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_PERI);
    uint f_clk_usb = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_USB);
    uint f_clk_adc = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_ADC);
    uint f_clk_rtc = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_RTC);

    printf("pll_sys  = %dkHz\n", f_pll_sys);
    printf("pll_usb  = %dkHz\n", f_pll_usb);
    printf("rosc     = %dkHz\n", f_rosc);
    printf("clk_sys  = %dkHz\n", f_clk_sys);
    printf("clk_peri = %dkHz\n", f_clk_peri);
    printf("clk_usb  = %dkHz\n", f_clk_usb);
    printf("clk_adc  = %dkHz\n", f_clk_adc);
    printf("clk_rtc  = %dkHz\n", f_clk_rtc);
    uart_default_tx_wait_blocking();
    // Can't measure clk_ref / xosc as it is the ref
}

int main() {
    stdio_init_all();
    printf("Starting\n");

    //save values for later
    uint scb_orig = scb_hw->scr;
    uint clock0_orig = clocks_hw->sleep_en0;
    uint clock1_orig = clocks_hw->sleep_en1;

    gpio_init(FLASH_LED);
    gpio_set_dir(FLASH_LED, GPIO_OUT);

    uint8_t slice_num = setupRain();
    setupWake();

    //crudely reset the clock each time
    //to the value below
    datetime_t t = {
            .year  = 2020,
            .month = 06,
            .day   = 05,
            .dotw  = 5, // 0 is Sunday, so 5 is Friday
            .hour  = 15,
            .min   = 45,
            .sec   = 00
    };

    // Start the Real time clock
    rtc_init();

    measure_freqs();
    uart_default_tx_wait_blocking();
    while (true) {
        gpio_put(FLASH_LED, 1);
        sleep_ms(2000);

        awake = false;
        printf("Sleep from xosc\n");
 	   printf("Count Pre Sleep is %u\n",
 			   pwm_get_counter(slice_num)
 			   );
        uart_default_tx_wait_blocking();

        sleep_run_from_xosc();
        //Reset real time clock to a value
        rtc_set_datetime(&t);

        gpio_put(FLASH_LED, 0);
        extWake = false;
        //sleep here, in this case for 1 min
        rtc_sleep(-1, 10);

        gpio_put(FLASH_LED, 1);


        //will return here and awake should be true
        while (!awake) {
            //This should not fire
            printf("Should be sleeping\n");
            uart_default_tx_wait_blocking();
        }

        //reset processor and clocks back to defaults
        recover_from_sleep(scb_orig, clock0_orig, clock1_orig);

   	   printf("Count Post Sleep is %u\n",
   			   pwm_get_counter(slice_num)
   			   );
   	   if (extWake){
   		   printf("EXTERNAL WAKE\n");
   	   }

        //clocks should be restored
        measure_freqs();



        //prove we can use sleep_ms once more without going into deep sleep
        //(this will not return unless recover_from_sleep() is ran as above)
        printf("Sleep from sleep_ms\n");
        uart_default_tx_wait_blocking();
        sleep_ms(2000);
    }
}
