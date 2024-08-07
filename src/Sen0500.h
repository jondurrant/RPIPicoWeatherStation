/*
 * Sen0500.h
 *
 * I2C access to the SEN0500 by DFRobot
 *
 *  Created on: 5 Aug 2024
 *      Author: jondurrant
 */

#ifndef EXP_SEN0500_SRC_SEN0500_H_
#define EXP_SEN0500_SRC_SEN0500_H_

#include "pico/stdlib.h"
#include "hardware/i2c.h"


class Sen0500 {
public:
	/***
	 * Constructor
	 * @param i2c - I2C interface being used. Default
	 * @param addr - Address being used. Default
	 */
	Sen0500(i2c_inst_t * i2c = i2c0,  uint16_t addr = 0x0022);
	virtual ~Sen0500();

	/***
	 * Read temperature in Celsius
	 * @param temp - Celsius
	 * @return True if completes ok.
	 */
	bool readTemp(float *temp);

	/***
	 *  Reads humidity as a percentage
	 * @param humid
	 * @return true if completes ok
	 */
	bool readHumid(float *humid);

	/***
	 *  Reads Atmospheric pressure in hPa
	 * @param atmosphere in hPa
	 * @return true if completes ok
	 */
	bool readPressure(uint16_t *atmosphere);

	/***
	 *  Reads UV Intensity  in mnW per m2
	 * @param intensity in mW per m2
	 * @return true if completes ok
	 */
	bool readUV(float *intensity);

	/***
	 *  Reads Luminus Intensity  in Lux
	 * @param lumi in lx
	 * @return true if completes ok
	 */
	bool readLumi(float *lumi);


	/***
	 *  Reads Firmware version
	 * @param version
	 * @return true if completes ok
	 */
	bool readVer(uint16_t *version);


private:

	uint8_t readReg(uint16_t reg, uint8_t *pBuf, uint8_t size);


	i2c_inst_t * pI2C;
	uint16_t xAddr;
};

#endif /* EXP_SEN0500_SRC_SEN0500_H_ */
