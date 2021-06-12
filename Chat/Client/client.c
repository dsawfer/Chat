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
#include "ClientParams.h"

pthread_mutex_t mutex;
pthread_mutex_t mutex_file;

void str_overwrite_stdout() {
	printf("%s", "");
	fflush(stdout);
}

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

		printf("%s\n", buff);
		str_overwrite_stdout();

		pthread_mutex_unlock(&mutex_file);
		pthread_mutex_unlock(&mutex);
		memset(buff, '\0', 1024);
	}
	return 0;
}

void* SendNewMassages(void* client_socket)
{
	SOCKET my_socket;
	my_socket = (SOCKET)client_socket;

	//char buff[1024];
	char message[1024];
	int ret = 0;

	while (ret != SOCKET_ERROR) {
		//printf("Client:");
		gets_s(message, 1024);
		//sprintf(buff, "%s: %s\n", name, message);
		send(my_socket, message, sizeof(message), 0);

		if (!strcmp(message, "/exit"))
		{
			break;
		}

		//memset(buff, '\0', 1024);
		memset(message, '\0', 1024);
	}
	flag = 1;
}

void CreateClient()
{
	SOCKET client;
	//char buff[1024];
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

	gets_s(name, 30);
	if (!strlen(name) || strlen(name) > 30) {
		printf("Incorrect name, connection close\n");
		return;
	}
	send(client, name, sizeof(name), 0);

	pthread_mutex_init(&mutex, NULL);
	pthread_mutex_init(&mutex_file, NULL);
	pthread_t getMessages;		//поток для проверки нового сообщения
	int getStatus = pthread_create(&getMessages, NULL, GetNewMassages, (void*)client);

	//int ret = 0;
	//ret = recv(client, buff, sizeof(buff) - 1, 0);
	//printf("%s\n", buff);

	pthread_t sendMessages;		//поток для отправки нового сообщения
	int Sendstatus = pthread_create(&sendMessages, NULL, SendNewMassages, (void*)client);
	//

	while (1)
	{
		if (flag == 1) {
			pthread_detach(getMessages);
			pthread_detach(sendMessages);

			closesocket(client);
			WSACleanup();

			pthread_mutex_destroy(&mutex_file);
			pthread_mutex_destroy(&mutex);
			break;
		}
	}

	
	//return;
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