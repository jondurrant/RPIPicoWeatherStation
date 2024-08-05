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

#define UART_ID uart1
#define BAUD_RATE 9600
#define UART_TX_PIN 20
#define UART_RX_PIN 21


void flash(uint count=1){
	const uint LED_PIN = LED_PAD;

	for (uint i=0; i < count; i++){
		gpio_put(LED_PIN, 1);
		sleep_ms(DELAY);
		gpio_put(LED_PIN, 0);
		sleep_ms(DELAY);
	}
}

template<size_t s>
struct sRtuPacketHeader {
  uint16_t len;
  uint8_t id;
  uint8_t cmd;
 uint8_t payload[s];
  uint16_t cs;
}  ;

typedef  sRtuPacketHeader<0> sRtuPacketHeader_t, *pRtuPacketHeader_t;

uint16_t calculateCRC(uint8_t *data, uint8_t len){
  uint16_t crc = 0xFFFF;
  for( uint8_t pos = 0; pos < len; pos++){
    crc ^= (uint16_t)data[ pos ];
    for(uint8_t i = 8; i != 0; i--){
      if((crc & 0x0001) != 0){
        crc >>= 1;
        crc ^= 0xA001;
      }else{
         crc >>= 1;
      }
    }
  }
  crc = ((crc & 0x00FF) << 8) | ((crc & 0xFF00) >> 8);
  return crc;
}

pRtuPacketHeader_t  packed(uint8_t id, uint8_t cmd, void *data, uint16_t size){
  pRtuPacketHeader_t header = NULL;
  uint16_t crc = 0;
  if((data == NULL) || (size == 0)) return NULL;
  if((header = (pRtuPacketHeader_t)malloc(sizeof(sRtuPacketHeader_t) + size)) == NULL){
    printf("Memory ERROR");
    return NULL;
  }
  header->len = sizeof(sRtuPacketHeader_t) + size - 2;
  header->id = id;
  header->cmd = cmd;
  memcpy(header->payload, data, size);
  crc = calculateCRC((uint8_t *)&(header->id), (header->len) - 2);
  ((uint8_t *)header->payload)[size] = (crc >> 8) & 0xFF;
  ((uint8_t *)header->payload)[size+1] = crc & 0xFF;
  return header;
}

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

    //Setup LED
    const uint LED_PIN = LED_PAD;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);

    flash(5);


    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);



   for (;;){

	   uint reg = 0x000A;
	   uint regNum = 2;
	   uint8_t temp[4] = {
			   (uint8_t)((reg >> 8) & 0xFF),
			   (uint8_t)(reg & 0xFF),
			   (uint8_t)((regNum >> 8) & 0xFF),
			   (uint8_t)(regNum & 0xFF)
	   };

	   uint8_t d[2];
	   uint8_t id = 0x22;
	   uint8_t cmd = 0x04;

	   pRtuPacketHeader_t p = packed(
			   id,
			   cmd,
			   d,
			   2);


	   debugPrintBuffer("Send", (const uint8_t *)p,
	   	   	   sizeof(*p)
			   );
	   uart_write_blocking (
			   UART_ID,
			   (const uint8_t *)p,
	   	   	   sizeof(*p)
			   );


	   while (uart_is_readable(UART_ID)){
		   char c = uart_getc(UART_ID);
		   printf("%c", c);
	   }
	   printf("\nEnd of Data\n");

	   sleep_ms(1000);
	   flash(5);
   }

}
