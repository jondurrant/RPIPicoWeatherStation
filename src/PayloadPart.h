/*
 * PayloadPart.h
 *
 *  Created on: 12 Sept 2024
 *      Author: jondurrant
 */

#ifndef SRC_PAYLOADPART_H_
#define SRC_PAYLOADPART_H_

class PayloadPart {
public:
	PayloadPart();
	virtual ~PayloadPart();

	virtual char* writeJson( char* dest,const  char * name, size_t* remLen ) = 0;
};

#endif /* SRC_PAYLOADPART_H_ */
