/*
 * ConfigRequest.h
 *
 *  Created on: 8 Dec 2024
 *      Author: jondurrant
 */

#ifndef SRC_CONFIGREQUEST_H_
#define SRC_CONFIGREQUEST_H_

#include "JSONSerialisable.h"

class ConfigRequest :public JSONSerialisable {
public:
	ConfigRequest();
	virtual ~ConfigRequest();

	virtual char * json();

private:
	char xJSON[256];
};

#endif /* SRC_CONFIGREQUEST_H_ */
