#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include <ctype.h>
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

int add(int a, int b)
{
	return a + b;
}

int sub(int a, int b)
{
	return a - b;
}

int mult(int a, int b)
{
	return a * b;
}

int division(int a, int b)
{
	if (b != 0)
		return a / b;
	else
	{
		showMex("[-] Division Error.");
		return 0;
	}
}

//it return a result according to the operation 'op'
int compute(char op, int f, int s)
{
	int r = 0;
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

	//initialize server sock and client sock
	if (initialize() == 1)
	{
		int server_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

		//setting server structure
		struct sockaddr_in sad; //structure for the server address
		sad.sin_family = AF_INET;
		sad.sin_addr.s_addr = inet_addr(addr);
		sad.sin_port = htons(port);

		//binding server
		if (bind(server_sock, (struct sockaddr*) &sad, sizeof(sad)) <0)
		{
			showMex("[-] Server Bind failed.");
			closesocket(server_sock);
			clearWinSock();
			system("pause");
			return -1;
		}
		printf("[+] Server started on %s : %d\n", addr, port);

		//server loop
		while (1)
		{
			//listen()
			if (listen(server_sock, CLIENTSIZE) < 0)
			{
				showMex("[-] Listen() Function Failed.");
				closesocket(server_sock);
				clearWinSock();
				system("pause");
				return -1;
			}

			//accept client
			struct sockaddr_in cad; //structure for the client address
			int client_sock; //socket descriptor for the client
			int client_len; //the size of the client address
			showMex( "[+] Waiting for a client to connect...");

			Message mex;
			char operation;
			int first, second, response;

			client_len = sizeof(cad);
			if ((client_sock = accept(server_sock, (struct sockaddr *) &cad, &client_len)) < 0)
			{
				showMex("[-] Client Refused Connection.");
				closesocket(client_sock);
				clearWinSock();
				system("pause");
				return -1;
			}

			char welcome[] = "Connection set.";
			if (send(client_sock, welcome, strlen(welcome), 0) < 0)
			{
				showMex("[-] Cannot send data to the client.");
				closesocket(client_sock);
				clearWinSock();
				system("pause");
				return -1;
			}

			printf("[+] Connection established with %s : %d\n", inet_ntoa(cad.sin_addr), ntohs(cad.sin_port));

			//application loop
			while (1)
			{
				char res[BUFFERSIZE];
				if ((recv(client_sock, &mex, sizeof(mex), 0)) < 0)
				{
					showMex("[-] Error while receiving data from the client.");
					closesocket(client_sock);
					clearWinSock();
					system("pause");
					return -1;
				}
				operation = mex.operation;
				if (operation == '=')
				{
					showMex("[-] Client disconnected from the server.");
					break;
				}
				first = ntohs(mex.first);
				second = ntohs(mex.second);

				response = compute(operation, first, second);
				sprintf(res, "%d", response);
				if (send(client_sock, res, strlen(res), 0) < 0)
				{
					showMex("[-] Cannot send data to the client.");
					closesocket(client_sock);
					clearWinSock();
					system("pause");
					return -1;
				}
			}
		}
	}
	else
	{
		showMex("[-] Error while initializing winsock.");
		clearWinSock();
		system("pause");
		return -1;
	}

	system("pause");
	return 0;
}
