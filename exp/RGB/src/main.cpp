
#include <stdio.h>
#include "pico/stdlib.h"


#define SWT 6
#define RED 7
#define GRN 10
#define BLU 11

uint8_t col = 0;
uint32_t last = 0;


void gpio_callback(uint gpio, uint32_t events) {

	uint32_t now =  to_ms_since_boot (get_absolute_time());

	if ((now - last) < 300){
		return;
	}

	last = now;

	col ++;
	if (col > 2){
		col = 0;
	}

	switch (col){
	case 0:
		gpio_put(RED, true);
		gpio_put(GRN, false);
		gpio_put(BLU, false);
		break;
	case 1:
		gpio_put(RED, false);
		gpio_put(GRN, true);
		gpio_put(BLU, false);
		break;
	case 2:
		gpio_put(RED, false);
		gpio_put(GRN, false);
		gpio_put(BLU, true);
		break;
	}

}

int main() {

    stdio_init_all();



     gpio_init(RED);
     gpio_set_dir(RED, GPIO_OUT);
     gpio_put(RED, false);

     gpio_init(GRN);
     gpio_set_dir(GRN, GPIO_OUT);
     gpio_put(GRN, false);

     gpio_init(BLU);
     gpio_set_dir(BLU, GPIO_OUT);
     gpio_put(BLU, false);


     gpio_init(SWT);
     gpio_pull_up(SWT);
     gpio_set_dir(SWT, GPIO_IN);
     gpio_set_irq_enabled_with_callback(
    		 SWT,
			 GPIO_IRQ_EDGE_FALL,
			 true,
			 &gpio_callback
			 );


	 gpio_put(RED, true);



     for (;;){


    	 sleep_ms(1000);
     }

 }
