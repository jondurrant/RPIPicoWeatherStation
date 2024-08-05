/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

// Sweep through all 7-bit I2C addresses, to see if any slaves are present on
// the I2C bus. Print out a table that looks like this:
//
// I2C Bus Scan
//   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
// 0
// 1       @
// 2
// 3             @
// 4
// 5
// 6
// 7
//
// E.g. if slave addresses 0x12 and 0x34 were acknowledged.

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"

#define I2CCHAN i2c0
#define SDA_PAD 12
#define SCL_PAD 13

#define REG_ULTRAVIOLET_INTENSITY 0x0008 ///< Register for protocol transition adapter
#define REG_LUMINOUS_INTENSITY    0x0009 ///< Register for protocol transition adapter
#define REG_TEMP                  0x000A ///< Register for protocol transition adapter
#define REG_HUMIDITY              0x000B ///< Register for protocol transition adapter
#define REG_ATMOSPHERIC_PRESSURE  0x000C ///< Register for protocol transition adapter
#define REG_ELEVATION

// I2C reserves some addresses for special purposes. We exclude these from the scan.
// These are any addresses of the form 000 0xxx or 111 1xxx
bool reserved_addr(uint8_t addr) {
    return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
}

int main() {
    // Enable UART so we can print status output
    stdio_init_all();

    // This example will use I2C0 on the default SDA and SCL pins (GP4, GP5 on a Pico)
    i2c_init(I2CCHAN, 100 * 1000);
    gpio_set_function(SDA_PAD, GPIO_FUNC_I2C);
    gpio_set_function(SCL_PAD, GPIO_FUNC_I2C);
    gpio_pull_up(SDA_PAD);
    gpio_pull_up(SCL_PAD);
    // Make the I2C pins available to picotool
   // bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));

    printf("\nI2C Bus Scan\n");
    printf("   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");

    for (int addr = 0; addr < (1 << 7); ++addr) {
        if (addr % 16 == 0) {
            printf("%02x ", addr);
        }

        // Perform a 1-byte dummy read from the probe address. If a slave
        // acknowledges this address, the function returns the number of bytes
        // transferred. If the address byte is ignored, the function returns
        // -1.

        // Skip over any reserved addresses.
        int ret;
        uint8_t rxdata;
        if (reserved_addr(addr))
            ret = PICO_ERROR_GENERIC;
        else
            ret = i2c_read_blocking(I2CCHAN, addr, &rxdata, 1, false);

        printf(ret < 0 ? "." : "@");
        printf(addr % 16 == 15 ? "\n" : "  ");
    }
    printf("Done.\n");

    uint8_t reg[2] = {0x0A * 2, 0x0A};
    //uint16_t reg = REG_TEMP;
    //uint8_t data[2];
    uint16_t data;
    uint8_t buffer[2];
     float temp;

    int ret = i2c_write_timeout_us	(
    		I2CCHAN,
    		0x0022,
			reg,
			1,
			true,
			10000);
    if (ret != 1){
    	printf("Failed to write (%d)\n", ret);
    }else {
    	 ret = i2c_read_blocking(I2CCHAN, 0x0022,  buffer, 2, false);
    	 if (ret > 0){
    		 printf("Read (%d) %X, %X\n", ret, buffer[0], buffer[1]);
    		 data = buffer[0] << 8 | buffer[1];
    		 temp = (-45) +((data * 175.00) / 1024.00 / 64.00);
    		 //printf("Read(%d) = %X\n", ret, data);
    		 printf("Temp is %f X\n", temp);
    	 } else {
    		 printf("Read failed %d\n", ret);
    	 }
    }



    return 0;

}
