/*
 * SDLog.cpp
 *
 *  Created on: 7 Sept 2024
 *      Author: jondurrant
 */

#include "SDLog.h"

#include "ff_headers.h"
#include "ff_sddisk.h"
#include "ff_stdio.h"
#include "ff_utils.h"
#include "ff_dir.h"
//
#include "hw_config.h"
#include "ff_file.h"

SDLog::SDLog() {
	//init();

}

SDLog::~SDLog() {
	// TODO Auto-generated destructor stub
}

bool SDLog::init(){
	pxDisk = FF_SDDiskInit("sd0");
	return pxDisk != NULL;
}


bool SDLog::mount(){
	FF_Error_t xError = FF_SDDiskMount(pxDisk);
	if (FF_isERR(xError) != pdFALSE) {
		printf("FF_SDDiskMount: %s (%d)\n",
			  (const char *)FF_GetErrMessage(xError), xError);
		return false;
	} else {
		FF_FS_Add("/sd0", pxDisk);
		xDirScan = false;
		return true;
	}
}


bool SDLog::unmount(){
	FF_FS_Remove("/sd0");
	FF_Unmount(pxDisk);
	FF_SDDiskDelete(pxDisk);
	return true;
}


bool SDLog::list(const char * folder){
	bool reset = false;
		char file[256];

		printf("List files\n");

		if ( ff_findfirst( folder,  &xDirData ) != 0){
			printf("List Failed\n");
			return false;
		}

		for (;;){
			printf("File %s\n", xDirData.pcFileName);

			if ( ff_findnext(  &xDirData ) != 0){
				break;
			}
		}
		return true;
}

