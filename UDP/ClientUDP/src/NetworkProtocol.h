/*
 * NetworkProtocol.h
 *
 *  Created on: 9 dic 2021
 *      Author: Rosanna
 */

#ifndef NETWORKPROTOCOL_H_
#define NETWORKPROTOCOL_H_

typedef struct Message
{
	char operation;
	double first ;
	double second;
}Message;

#define NO_ERROR 0
#define ECHOMAX 255
#define PORT 56700
#define ADDRESS "127.0.0.1"
#define BUFFERSIZE 1024


#endif /* NETWORKPROTOCOL_H_ */
