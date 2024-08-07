/*
 * Sen0500.cpp
 *
 *  Created on: 5 Aug 2024
 *      Author: jondurrant
 */

#include "Sen0500.h"

#define REG_ULTRAVIOLET_INTENSITY 	0x08
#define REG_LUMINOUS_INTENSITY    		0x09
#define REG_TEMP                  								0x0A
#define REG_HUMIDITY             	 						0x0B
#define REG_ATMOSPHERIC_PRESSURE  0x0C
#define REG_VERSION											0x05

Sen0500::Sen0500(i2c_inst_t * i2c,  uint16_t addr ) {
	xAddr = addr;
	pI2C = i2c;
}

Sen0500::~Sen0500() {
	// TODO Auto-generated destructor stub
}


uint8_t Sen0500::readReg(uint16_t reg, uint8_t *pBuf, uint8_t size){
	uint8_t r = reg * 2;
	int ret = i2c_write_timeout_us	(
			pI2C,
			xAddr,
			&r,
			1,
			true,
			10000);

	     if (ret != 1){
	     	return 0;
	     }else {
	     	 ret = i2c_read_blocking(pI2C, xAddr,  pBuf,  size, false);
	     	 return ret;
	     }
}


bool Sen0500::readTemp(float *temp){
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


bool Sen0500::readHumid(float *humid){
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

bool Sen0500::readPressure(uint16_t *atmosphere){
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

bool Sen0500::readVer(uint16_t *version){
    uint16_t data;
    uint8_t buffer[2];

	int ret = readReg(REG_VERSION, buffer, 2);
	if (ret == 2){
		*version = buffer[0] << 8 | buffer[1];
		 return true;
	}
	return false;
}

bool Sen0500::readUV(float *intensity){
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

bool Sen0500::readLumi(float *lumi){
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

