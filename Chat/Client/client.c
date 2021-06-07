// client.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

//#include "pch.h"
#define HAVE_STRUCT_TIMESPEC
#pragma comment(lib, "ws2_32.lib")
#define _CRT_SECURE_NO_WARNINGS
#include <winsock.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

void SendData2Server(int count, int number)
{
	SOCKET client;
	client = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (client == INVALID_SOCKET)
	{
		printf("Error create socket\n");
		return;
	}
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(5510); //the same as in server
	server.sin_addr.S_un.S_addr = inet_addr("127.0.0.1"); //special look-up address
	if (connect(client, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("Can't connect to server\n");
		closesocket(client);
		return;
	}
	char message[1024];
	sprintf(message, "<%d client> %s %d", number, "test", count);
	int ret = send(client, message, strlen(message), 0);
	if (ret == SOCKET_ERROR)
	{
		printf("Can't send message\n");
		closesocket(client);
		return;
	}
	printf("Sent: %s\nbytes: %d\n\n", message, ret);
	ret = SOCKET_ERROR;
	int i = 0;
	while (ret == SOCKET_ERROR)
	{
		//полчение ответа
		ret = recv(client, message, 1024, 0);
		//обработка ошибок
		if (ret == 0 || ret == WSAECONNRESET)
		{
			printf("Connection closed\n");
			break;
		}
		if (ret < 0)
		{
			//printf("Can't resieve message\n");
			/*closesocket(client);
			return;*/
			continue;
		}
		//вывод на экран количества полученных байт и сообщение
		printf("Recieve: %s\n bytes: %d\n", message, ret);
	}
	closesocket(client);
}

int main()
{
	WSADATA wsd;
	if (WSAStartup(MAKEWORD(1, 1), &wsd) != 0)
	{
		printf("Can't connect to socket lib");
		return 1;
	}
	int i = 0;
	srand(time(0));
	rand();
	int number = rand();
	while (i < 1000)
	{
		SendData2Server(++i, number);
		Sleep(rand() % 10);
	}
	printf("Session is closed\n");
	Sleep(1000);
	return 0;
}

