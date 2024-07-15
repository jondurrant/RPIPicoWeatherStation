/**
 * Simple Hibernate and Recovery on a Raspberry PI Pico
 * LED is flashed on GPIO 2 while a wake
 *
 * RTC DS3231 connected on I2C to GP12 & 13
 * RTC SQW used for interupt to wake on GP10
 */

#include "pico/stdlib.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>



#define LED_PAD 2
#define DELAY 500 // in microseconds
#define SDA_PAD 12
#define SCL_PAD 13

#define WAKE_PAD 10

#define BAUD_RATE 9600
#define UART_RX1_PIN 1
#define UART_RX2_PIN 4




#define DEBUG_LINE 25

void debugPrintBuffer(const char *title, const void * pBuffer, size_t bytes){
	size_t count =0;
	size_t lineEnd=0;
	const uint8_t *pBuf = (uint8_t *)pBuffer;

	printf("DEBUG: %s of size %d\n", title, bytes);

	while (count < bytes){
		lineEnd = count + DEBUG_LINE;
		if (lineEnd > bytes){
			lineEnd = bytes;
		}

		//Print HEX DUMP
		for (size_t i=count; i < lineEnd; i++){
			if (pBuf[i] <= 0x0F){
				printf("0%X ", pBuf[i]);
			} else {
				printf("%X ", pBuf[i]);
			}
		}

		//Pad for short lines
		size_t pad = (DEBUG_LINE - (lineEnd - count)) * 3;
		for (size_t i=0; i < pad; i++){
			printf(" ");
		}

		//Print Plain Text
		for (size_t i=count; i < lineEnd; i++){
			if ((pBuf[i] >= 0x20) && (pBuf[i] <= 0x7e)){
				printf("%c", pBuf[i]);
			} else {
				printf(".");
			}
		}

		printf("\n");

		count = lineEnd;

	}
}


int main() {
	uint resurrect = 0;
    stdio_init_all();
    sleep_ms(2000);
    printf("GO\n");


    uart_init(uart0, BAUD_RATE);
    uart_init(uart1, BAUD_RATE);
    gpio_set_function(UART_RX1_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX2_PIN, GPIO_FUNC_UART);


    char buf[1024];
    uint len=0;

   for (;;){

	   len = 0;
	   while (uart_is_readable(uart0)){
		   char c = uart_getc(uart0);
		   buf[len++] = c;
	   }
	   if (len > 0){
		   debugPrintBuffer("UART0", buf, len);
	   } else {
		   printf("UART0 NO DATA\n");
	   }

	   len = 0;
	   while (uart_is_readable(uart1)){
		   char c = uart_getc(uart1);
		   buf[len++] = c;
	   }
	   if (len > 0){
		   debugPrintBuffer("UART1", buf, len);
	   } else {
		   printf("UART1 NO DATA\n");
	   }

	   sleep_ms(1000);
   }

}
