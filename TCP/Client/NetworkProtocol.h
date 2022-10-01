/*
 * NetworkProtocol.h
 *
 *  Created on: 11 nov 2021
 *      Author: danie
 */

#ifndef NETWORKPROTOCOL_H_
#define NETWORKPROTOCOL_H_

#define PORT 60000
#define ADDRESS "127.0.0.1"
#define BUFFERSIZE 1024
#define CLIENTSIZE 5

typedef struct Message
{
	char operation;
	int first;
	int second;
}Message;


#endif /* NETWORKPROTOCOL_H_ */
