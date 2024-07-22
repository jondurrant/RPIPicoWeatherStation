/**
 * Check batter usage
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include <pico/stdlib.h>

#include "hardware/adc.h"

#if CYW43_USES_VSYS_PIN
#include "pico/cyw43_arch.h"
#endif

extern "C" {
#include <power_status.h>
}


int main() {
    stdio_init_all();

    adc_init();

    // Pico W uses a CYW43 pin to get VBUS so we need to initialise it
    #if CYW43_USES_VSYS_PIN
    if (cyw43_arch_init()) {
        printf("failed to initialise\n");
        return 1;
    }
    #endif

    bool vsys;
    float volts;

    for (;;){

        if (power_source(&vsys) == PICO_OK) {
        	if (vsys){
        		printf("VSYS \n");
        	} else {
        		printf("VBUS \n");
        	}
        }


        int voltage_return = power_voltage(&volts);
        printf("Volts %.2f\n", volts);

        sleep_ms(1000);
    }

#if CYW43_USES_VSYS_PIN
    cyw43_arch_deinit();
#endif
    return 0;
}
