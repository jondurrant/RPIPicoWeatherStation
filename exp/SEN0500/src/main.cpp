
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include "Sen0500.h"

#define I2CCHAN i2c0
#define SDA_PAD 12
#define SCL_PAD 13

#define REG_ULTRAVIOLET_INTENSITY 0x0008 ///< Register for protocol transition adapter
#define REG_LUMINOUS_INTENSITY    0x0009 ///< Register for protocol transition adapter
#define REG_TEMP                  0x000A ///< Register for protocol transition adapter
#define REG_HUMIDITY              0x000B ///< Register for protocol transition adapter
#define REG_ATMOSPHERIC_PRESSURE  0x000C ///< Register for protocol transition adapter

#define SEN0500_ADDR 0x0022



uint8_t readReg(uint16_t reg, uint8_t *pBuf, uint8_t size){
	uint8_t r = reg * 2;
	int ret = i2c_write_timeout_us	(
			I2CCHAN,
			SEN0500_ADDR,
			&r,
			1,
			true,
			10000);

	     if (ret != 1){
	     	return 0;
	     }else {
	     	 ret = i2c_read_blocking(I2CCHAN, 0x0022,  pBuf,  size, false);
	     	 return ret;
	     }
}

bool readTemp(float *temp){
    uint16_t data;
    uint8_t buffer[2];

	int ret = readReg(REG_TEMP, buffer, 2);
	if (ret == 2){
		data = buffer[0] << 8 | buffer[1];
		 *temp = (-45) +((data * 175.00) / 1024.00 / 64.00);
		 return true;
	}
	return false;
}

bool readHumid(float *humid){
    uint16_t data;
    uint8_t buffer[2];

	int ret = readReg(REG_HUMIDITY, buffer, 2);
	if (ret == 2){
		data = buffer[0] << 8 | buffer[1];
		 *humid = ((data * 100.00) / 65536.00);
		 return true;
	}
	return false;
}

bool readPressure(uint16_t *atmosphere){
    uint16_t data;
    uint8_t buffer[2];

	int ret = readReg(REG_ATMOSPHERIC_PRESSURE, buffer, 2);
	if (ret == 2){
		data = buffer[0] << 8 | buffer[1];
		 *atmosphere = data;
		 return true;
	}
	return false;
}

bool readVer(uint16_t *version){
    uint16_t data;
    uint8_t buffer[2];

	int ret = readReg(0x05, buffer, 2);
	if (ret == 2){
		*version = buffer[0] << 8 | buffer[1];
		 return true;
	}
	return false;
}

bool readUV(float *intensity){
    uint16_t data;
    uint8_t buffer[2];

	int ret = readReg(REG_ULTRAVIOLET_INTENSITY, buffer, 2);
	if (ret == 2){
		data = buffer[0] << 8 | buffer[1];
		 *intensity = (float)data/1800.0;
		 return true;
	}
	return false;
}

bool readLumi(float *lumi){
    uint16_t data;
    uint8_t buffer[2];

	int ret = readReg(REG_ULTRAVIOLET_INTENSITY, buffer, 2);
	if (ret == 2){
		data = buffer[0] << 8 | buffer[1];
		float luminous = data;
		luminous = luminous * (1.0023f + luminous * (8.1488e-5f + luminous * (-9.3924e-9f + luminous * 6.0135e-13f)));
		 *lumi = luminous;
		 return true;
	}
	return false;
}


int main() {

    stdio_init_all();

    // This example will use I2C0 on the default SDA and SCL pins (GP4, GP5 on a Pico)
     i2c_init(I2CCHAN, 100 * 1000);
     gpio_set_function(SDA_PAD, GPIO_FUNC_I2C);
     gpio_set_function(SCL_PAD, GPIO_FUNC_I2C);
     gpio_pull_up(SDA_PAD);
     gpio_pull_up(SCL_PAD);


     float temp;
     float humid;
     uint16_t atmos;
     uint16_t ver;
     float uv;
     float lumi;

     Sen0500 sen(I2CCHAN);


     for (;;){

    	 sen.readTemp(&temp);
    	 sen.readHumid(&humid);
    	 sen.readPressure(&atmos);
    	 sen.readUV(&uv);
    	 sen.readLumi(&lumi);

    	 printf("%0.2f C, %0.2f%%, %dhPa, %0.5fmW/M2, %0.5flx\n",
    			 temp,
				 humid,
				 atmos,
				 uv,
				lumi
    			 );


    	 /*
    	 printf("\n");
    	 if (readVer(&ver)){
    		 printf("Firmware version %X\n", ver);
    	 }

    	 if(readTemp(&temp)){
    		 printf("Temp %0.2f\n", temp);
    	 } else {
    		 printf("Read failed\n");
    	 }

    	 if(readHumid(&humid)){
			 printf("Humid %0.2f %%\n", humid);
		 }


    	 if(readPressure(&atmos)){
			 printf("Pressure %d hPa \n", atmos);
		 }

    	 if (readUV(&uv)){
    		 printf("UV Intensity %.5f mW/m2\n", uv);
    	 }

    	 if (readLumi(&lumi)){
    		 printf("Lumi %.5f lx \n ", lumi);
    	 }
    	 */




    	 sleep_ms(1000);
     }

 }
