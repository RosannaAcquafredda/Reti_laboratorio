/*
 ============================================================================
 Name        : ServerUDP.c
 Description : Server application that:
 	 	 	 	 - receives message from client
 	 	 	 	 - computes the operation requested
 	 	 	 	 - sends output back to the client
 ============================================================================
 */

#if defined WIN32
#include <winsock.h>
#else
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#define closesocket close
#endif

#include <string.h>
#include <math.h>
#include <stdio.h>
#include "NetworkProtocol.h"

void ErrorHandler(char *errorMessage){
	printf(errorMessage);
}

int Initialize(){
	#if defined WIN32
	WSADATA wsa_data;
	int result = WSAStartup(MAKEWORD(2,2), &wsa_data);
	if (result != NO_ERROR) {
		return -1;
	}else{
		return 1;
	}
	#endif
}

void ClearWinsock() {
	#if defined WIN32
		WSACleanup();
	#endif
}


double add(double a, double b){
	return a + b;
}

double sub(double a, double b){
	return a - b;
}

double mult(double a, double b){
	return a * b;
}

double division(double a, double b){
	if (b != 0)
		return a / b;
	else
	{
		ErrorHandler("[-] Division Error.");
		return 0;
	}
}

//switch that solve operations
double compute(char op, double f, double s){
	double r = 0;
	switch(op)
	{
	case '+':
		r = add(f, s);
		break;
	case '-':
		r = sub(f, s);
		break;
	case '*':
		r = mult(f, s);
		break;
	case '/':
		r = division(f, s);
		break;
	case '=':
		break;
	}
	 return r;

}

int main(int argc, char *argv[]){

	//setting up server and port
	char *addr;
	int port;
	if(argc <= 1){
		//there's no parameters
		//use default ip address and port number for startng server
		addr = (char *) malloc (strlen(ADDRESS) * sizeof(ADDRESS));
		strcpy(addr, ADDRESS);
		port = PORT;

	}else{

		if (argc <= 2) {
			//if there's only ip
			addr = (char *) malloc (strlen(argv[1]) * sizeof(argv[1]));
			strcpy(addr, argv[1]);
			port = PORT; //use default port
		}
		else
		{
			//if user start the application with two parameters - ip and port
			addr = (char *) malloc (strlen(argv[1]) * sizeof(argv[1]));
			strcpy(addr, argv[1]);
			port = atoi(argv[2]);
		}

	}

	//initialization
	if(Initialize()==1){

		struct sockaddr_in serverAddr;
		struct sockaddr_in clientAddr;
		unsigned int clientLen;

		//creating socket
		int ssock;
		if((ssock = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP))<0){
			ErrorHandler("\n[-]Socket creation failed.");
			closesocket(ssock);
			ClearWinsock();
			return -1;
		}

		//setting up server address
		memset(&serverAddr, 0, sizeof(serverAddr));
		serverAddr.sin_family =AF_INET;
		serverAddr.sin_port=htons(port);
		serverAddr.sin_addr.s_addr= inet_addr(addr);

		//socket bind
		if(bind(ssock,(struct sockaddr *)&serverAddr,sizeof(serverAddr))<0){
			ErrorHandler("bind() failed");
			closesocket(ssock);
			ClearWinsock();
			system("pause");
			return -1;
		}
		printf("\n[+] Server started on %s : %d\n", addr, port);

		//server loop
		while(1){

			Message mex;
			char operation;
			double first=0;
			double second = 0;
			double response = 0;
			char respMex[50] = {0} ;
			char tostring[10];
			const char ch = '=';
			double a,b;

			//processing client request
			while(1){

				//receiving data from client
				clientLen = sizeof(clientAddr);


				if((recvfrom(ssock,&mex, sizeof(mex), 0,(struct sockaddr *)&clientAddr, &clientLen))<0){
					ErrorHandler("\n[-]Error while receiving data from client");
					closesocket(ssock);
					ClearWinsock();
					system("pause");
					return -1;
				}

				//getting client name and IP address
				struct hostent *host;
				host=gethostbyaddr((char *)&clientAddr.sin_addr,sizeof(struct in_addr),AF_INET);
				char s[50];
				strcpy(s, host->h_name);
				char *ip = inet_ntoa(clientAddr.sin_addr);

				printf("\n[+] Task request: %c %d %d from client %s , IP  %s", mex.operation, ntohl(mex.first), ntohl(mex.second),s,ip);

				operation = mex.operation;
				if (operation == '='){
					ErrorHandler("\n[-] Client disconnected from the server.");
					break;
				}


				first = ntohl(mex.first);
				second = ntohl(mex.second);
				response = compute(operation, first, second);

				//setting up output message string
				sprintf(tostring,"%.f",first);
				strcat(respMex,tostring);
				strncat(respMex, &operation, 1);
				sprintf(tostring,"%.f",second);
				strcat(respMex,tostring);
				strncat(respMex,&ch,1);//=
				a = modf(response,&b);
				if(a == 0.00){
					sprintf(tostring,"%.f",response);
				}else{
					sprintf(tostring,"%.2f",response);
				}

				strcat(respMex,tostring);


				//sending answer to client
				if(sendto(ssock, respMex, strlen(respMex), 0,(struct sockaddr *)&clientAddr, sizeof(clientAddr)) <0){
					ErrorHandler("\n[-]Cannot send data to client");
					closesocket(ssock);
					ClearWinsock();
					system("pause");
					return -1;

				}
				memset(respMex,0,sizeof(respMex));

			}

		}


	}else{
		ErrorHandler("\n[-]Error while initializing Winsock.");
		ClearWinsock();
		system("pause");
		return -1;
	}


	system("pause");
	return 0;
}
