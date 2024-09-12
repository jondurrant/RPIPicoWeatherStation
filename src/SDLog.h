/*
 * SDLog.h
 *
 *  Created on: 7 Sept 2024
 *      Author: jondurrant
 */

#ifndef SRC_SDLOG_H_
#define SRC_SDLOG_H_

#include "pico/stdlib.h"
#include <cstdio>
#include "ff_headers.h"
#include "ff_sddisk.h"
#include "ff_stdio.h"

class SDLog {
public:
	SDLog();
	virtual ~SDLog();

	bool init();

	bool mount();

	bool unmount();

	bool list(const char * folde = "/sd0");

private:
	FF_Disk_t *pxDisk = NULL;

	FF_FindData_t xDirData;
	bool xDirScan = false;
};

#endif /* SRC_SDLOG_H_ */

