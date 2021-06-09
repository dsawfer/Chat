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
#include <string.h>
#include <pthread.h>

pthread_mutex_t mutex;
pthread_mutex_t mutex_file;

void* GetNewMassages(void* client_socket)
{
	SOCKET my_socket;
	my_socket = (SOCKET)client_socket;

	char buff[1024];
	int ret = 0;

	while (ret != SOCKET_ERROR)
	{
		ret = recv(my_socket, buff, sizeof(buff) - 1, 0);
		buff[ret] = '\0';

		pthread_mutex_lock(&mutex);
		pthread_mutex_lock(&mutex_file);
		printf("Server:%s\n", buff);
		pthread_mutex_unlock(&mutex_file);
		pthread_mutex_unlock(&mutex);
		memset(buff, '\0', 1024);
	}
	return 0;
}

void CreateClient()
{
	SOCKET client;
	char buff[1024];
	client = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (client == INVALID_SOCKET)
	{
		printf("Error create socket\n");
		return;
	}
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(5510);							//the same as in server
	server.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");	//special look-up address
	if (connect(client, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("Can't connect to server\n");
		closesocket(client);
		return;
	}
	printf("Connection established\n");

	pthread_mutex_init(&mutex, NULL);
	pthread_mutex_init(&mutex_file, NULL);
	pthread_t mythread3;		//поток для проверки нового сообщения
	int status = pthread_create(&mythread3, NULL, GetNewMassages, (void*)client);

	int ret = 0;
	ret = recv(client, buff, sizeof(buff) - 1, 0);
	while (ret != SOCKET_ERROR) {
		//ret = recv(client, buff, sizeof(buff) - 1, 0);	//sizeof(buff) - 1
		//buff[ret] = '\0';
		//printf("Server:%s\n", buff);

		printf("Client:"); 
		gets_s(buff, 1024);

		if (!strcmp(buff, "/quit"))
		{
			printf("Exit...");
			closesocket(client);
			WSACleanup();
			return 0;
		}

		send(client, buff, sizeof(buff), 0);
		memset(buff, '\0', 1024);
	}
	printf("Recv error %d\n", WSAGetLastError());
	closesocket(client);
	WSACleanup();

	pthread_mutex_destroy(&mutex_file);
	pthread_mutex_destroy(&mutex);

	return -1;
}

int main()
{
	WSADATA wsd;
	if (WSAStartup(MAKEWORD(1, 1), &wsd) != 0)
	{
		printf("Can't connect to socket lib");
		return 1;
	}

	CreateClient();

	printf("Session is closed\n");
	Sleep(1000);
	return 0;
}