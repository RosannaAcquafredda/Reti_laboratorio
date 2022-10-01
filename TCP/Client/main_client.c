#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include "NetworkProtocol.h"

//initializing wsdata
int initialize()
{
	WSADATA wd;
	WORD wVersion = MAKEWORD(2, 2);
	int res = WSAStartup(wVersion, &wd);
	if (res != NO_ERROR)
		return -1;
	else
		return 1;
}

void clearWinSock()
{
	WSACleanup();
}

void showMex(char *msg)
{
	printf("%s\n", msg);
}

int main(int argc, char *argv[])
{
	//setting up server address and port
	char *addr;
	int port;
	if (argc <= 1) //if no addr and port
	{
		addr = (char *) malloc (strlen(ADDRESS) * sizeof(ADDRESS));
		strcpy(addr, ADDRESS);
		port = PORT;
	}
	else
	{
		if (strlen(argv[2]) < 0) //if only addr
		{
			addr = (char *) malloc (strlen(argv[1]) * sizeof(argv[1]));
			strcpy(addr, argv[1]);
			port = PORT;
		}
		else
		{
			addr = (char *) malloc (strlen(argv[1]) * sizeof(argv[1]));
			strcpy(addr, argv[1]);
			port = atoi(argv[2]);
		}
	}

	//initializing server socket
	if (initialize() == 1)
	{
		//creating socket
		int sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (sock < 0)
		{
			showMex("[-] Socket creation failed.");
			closesocket(sock);
			clearWinSock();
			return -1;
		}

		//server address
		struct sockaddr_in server;
		memset(&server, 0, sizeof(server));
		server.sin_family = AF_INET;
		server.sin_addr.s_addr = inet_addr(addr);	//by default 127.0.0.1 (localhost)
		server.sin_port = htons(port);	//by default 60000

		//connecting to the server
		if (connect(sock, (struct sockaddr*) &server, sizeof(server)) < 0)
		{
			showMex("[-] Failed to connect to the server.");
			closesocket(sock);
			clearWinSock();
			return -1;
		}

		//getting server response back
		char buf[BUFFERSIZE];
		Message mex;
		int data = 0;
		if ((data = recv(sock, buf, BUFFERSIZE, 0)) < 0)
		{
			showMex("[-] Error in receiving data from the server.");
			closesocket(sock);
			clearWinSock();
			return -1;
		}
		printf("[SERVER]: %s\n", buf);

		//application loop
		while (1)
		{
			char operation;
			int first, second;
			memset(buf, 0, sizeof(buf)); //server result

			fflush(stdin);	//flushing standard input
			//getting operation and numbers
			printf("Enter an operation {'+', '-', '*', '/'}, and two integers [ex. + 11 22]\nPress '=' to close connection:\n> ");
			scanf("%c %d %d", &operation, &first, &second);

			//closing connection if the operation is equals to '='
			if (operation == '=')
			{
				mex.operation = operation;
				if ((send(sock, &mex, sizeof(mex), 0)) < 0)
				{
					showMex("[-] Error in sending data to the server.");
					closesocket(sock);
					clearWinSock();
					return -1;
				}
				showMex("[-] Disconnecting from the server.");
				closesocket(sock);
				clearWinSock();
				system("pause");
				return 0;
			}

			/*printf("Enter first number (must be integer):\n> ");
			scanf("%d", &first);

			printf("Enter second number (must be integer):\n> ");
			scanf("%d", &second);*/

			//setting up the mex
			mex.operation = operation;
			mex.first = htons(first);
			mex.second = htons(second);

			//sending mex to the server
			if ((send(sock, &mex, sizeof(mex), 0)) < 0)
			{
				showMex("[-] Error in sending data to the server.");
				closesocket(sock);
				clearWinSock();
				return -1;
			}

			//getting response from the server
			if ((data = recv(sock, buf, BUFFERSIZE, 0)) < 0)
			{
				showMex("[-] Error in receiving data from the server.");
				closesocket(sock);
				clearWinSock();
				return -1;
			}

			printf("[SERVER] Response: %s\n", buf);
		}
	}
	else
	{
		showMex("[-] Error while initializing winsock.");
		clearWinSock();
		return -1;
	}

	system("pause");
	return 0;
}
