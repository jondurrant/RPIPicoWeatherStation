/*
 * ConfigRequest.cpp
 *
 *  Created on: 8 Dec 2024
 *      Author: jondurrant
 */

#include "ConfigRequest.h"
#include "pico/unique_id.h"
#include "stdio.h"
#include <cstring>

ConfigRequest::ConfigRequest() {
	sprintf(xJSON, "{\"type\": \"WeatherStation\", "
				"\"device\": \"");
    pico_get_unique_board_id_string (
				&xJSON[strlen(xJSON) ],
				2 * PICO_UNIQUE_BOARD_ID_SIZE_BYTES + 1);
    sprintf(&xJSON[strlen(xJSON)], "\"}");
}

ConfigRequest::~ConfigRequest() {
	// NOP
}

char * ConfigRequest::json(){
	return xJSON;
}

