/*
 ============================================================================
 Name        : ClientUDP.c
 Description : CLient application that:
 	 	 	 	 - reads input operation
 	 	 	 	 - sends the request to the server
 	 	 	 	 - reads answer from server
 	 	 	 	 - prints the response to user
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

char getOperation(char in[]){
	return in[0];
}

double getFirst(char in[]){
	char num[BUFFERSIZE];
	int i = 2, j= 0;
	while(in[i]!=' '){
		num[j]= in[i];
		j++;
		i++;
	}
	num[j]='\0';
	return atof(num);
}

double getSecond(char in[]){
	char num[BUFFERSIZE];
	int i= 2, j = 0, k= 0;
	while(in[i]!= ' '){
		i++;
	}
	k = i+1;
	while(in[k]!= '\0'){
		num[j] = in[k];
		k++;
		j++;
	}
	num[k]='\0';
	return atof(num);
}

int main(int argc, char *argv[]){

	//setting up server and port
	//parameters follow the format '_servername_:_port_'

	char *string;
	char *addrName;
	char *addr = NULL;
	int port;
	if(argc <= 1){
		//there's no parameters
		//use default ip address and port number
		addrName= "localhost";
		addr = (char *) malloc (strlen(ADDRESS) * sizeof(ADDRESS));
		strcpy(addr, ADDRESS);
		port = PORT;
		//printf("%s\n%d", addr,port);

	}else{
		//users specify parameters
		string = (char *)malloc(strlen(argv[1]) * sizeof(argv[1]));
		strcpy(string, argv[1]);
		//get server name
		addrName = strtok(string,":");
		//get port number
		port = atoi(strtok(NULL, ":"));
		//printf("%s\n%d", addrName,port);

	}

	//initialize Winsock
	if(Initialize()==1){

		if(addr == NULL){

			//get server address starting from name
			struct hostent *host;
			host = gethostbyname(addrName);
			if (host == NULL) {
				fprintf(stderr, "\n[-]Gethostbyname() failed.\n");
				exit(EXIT_FAILURE);
			} else {
				struct in_addr* ina = (struct in_addr*) host->h_addr_list[0];
				addr = inet_ntoa(*ina);
				//printf("Risultato di gethostbyname(%s): %s\n", addrName,addr);
			}
		}


		//socket creation
		int sock;
		if((sock = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP))<0){
			ErrorHandler("\n[-]Socket creation failed.");
			closesocket(sock);
			ClearWinsock();
			return -1;
		}


		//setting up server address
		struct sockaddr_in serverAddr;
		struct sockaddr_in fromAddr;
		unsigned int fromSize;

		memset(&serverAddr, 0, sizeof(serverAddr));
		serverAddr.sin_family = AF_INET;
		serverAddr.sin_port=htons(port);
		serverAddr.sin_addr.s_addr= inet_addr(addr);


		Message mex;
		char bufInput[BUFFERSIZE] ;
		char bufOutput[BUFFERSIZE];

		//application loop
		while(1){

			//buffer memset
			memset(bufInput, 0, sizeof(bufInput));

			//flushing standard stream
			fflush(stdin);
			fflush(stdout);

			//getting operation and number
			printf("\nEnter an operation {'+', '-', '*', '/'}, and two integers [ex. + 11 22]\nPress '=' to close :\n");
			fgets(bufInput, BUFFERSIZE,stdin);

			//setting up the message (mex)
			mex.operation =getOperation(bufInput);
			mex.first = htonl(getFirst(bufInput));
			mex.second = htonl(getSecond(bufInput));

			//closing connection when operation is '='
			if(mex.operation == '='){

				if(sendto(sock, &mex, sizeof(mex),0,(struct sockaddr *)&serverAddr, sizeof(serverAddr))<0)
				{
					ErrorHandler("\n[-]Error in sending data to server");
					closesocket(sock);
					ClearWinsock();
					return -1;
				}
				printf("\n\n[-]Disconnecting from the server.");
				closesocket(sock);
				ClearWinsock();
				system("pause");
				return 0;
			}


			//sending mex to the server
			if(sendto(sock, &mex, sizeof(mex),0,(struct sockaddr *)&serverAddr, sizeof(serverAddr))< 0){
				ErrorHandler("\n[-]Problem while sending message to  the server ");
				closesocket(sock);
				ClearWinsock();
				return -1;
			}

			int rispLen;
			unsigned int fromSize;

			//getting the response
			fromSize = sizeof(fromAddr);
			rispLen = recvfrom(sock, bufOutput, ECHOMAX,0,(struct sockaddr *)&fromAddr,&fromSize);

			//sender check
			if(serverAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr){
				fprintf(stderr, "\n[-]Error: received a packet from an unknownn source\n");
				closesocket(sock);
				ClearWinsock();
				exit(EXIT_FAILURE);
			}

			printf("\n[+] Result received from Server '%s' , IP %s : %s",addrName, addr,bufOutput);
			memset(bufOutput,0,sizeof(bufOutput));

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
