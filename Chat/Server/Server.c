// server.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

//#include "pch.h"
#define HAVE_STRUCT_TIMESPEC
#define _CRT_SECURE_NO_WARNINGS
#include <pthread.h>
#pragma comment(lib, "ws2_32.lib")
#include <winsock.h>
#include <stdio.h>
#include <Windows.h>
#include <string.h>
#include "Params.h"
#define clientHello "Hello, new member! This's our win socket chat!))\r\n"

#define PRINTNUSERS if (nclients)\
  printf("%d users online\n",nclients);\
  else printf("No User online\n");

pthread_mutex_t mutex;
pthread_mutex_t mutex_file;

void* ClientStart(void* client_socket)
{
	SOCKET my_socket;
	my_socket = (SOCKET)client_socket;

	char buff[1024];
	send(my_socket, clientHello, sizeof(clientHello), 0);

	int ret = 0;

	while (ret != SOCKET_ERROR) {
		ret = recv(my_socket, buff, sizeof(buff), 0);
		
		//message = buff;
		printf("received: %s\n", buff);
		strcpy(message, buff);
		//printf("global message1: %s\n", message);
		//send(my_socket, buff, ret, 0);
		memset(buff, '\0', 1024);
		//printf("buff1: %s\n", buff);
		
	}
	nclients--;
	printf("-disconnect\n"); 
	PRINTNUSERS
	closesocket(my_socket);
	return 0;
}

void* CheckMessage(void* client_socket)
{
	SOCKET* my_socket;
	my_socket = (SOCKET*)client_socket;

	char buff[1024] = { 0 };

	while (1) {
		if (strcmp(message, "//message")) {

			printf("global message: %s\n", message);
			strcpy(buff, message);

			for (int i = 0; i < nclients; i++)
			{
				//printf("global message4: %s\n", message);
				send(my_socket[i], buff, strlen(buff) * sizeof(char), 0);
			}
			
			strcpy(message, "//message");
			printf("global message after recovery: %s\n\n", message);
			memset(buff, '\0', 1024);
		}
	}
}

int CreateServer()
{
	SOCKET server;
	SOCKET client[50];
	struct sockaddr_in localaddr, clientaddr;
	int size;
	server = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
	if (server == INVALID_SOCKET)
	{
		printf("Error create server\n");
		return 1;
	}
	localaddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	localaddr.sin_family = AF_INET;
	localaddr.sin_port = htons(5510);		//port number is for example, must be more than 1024
	if (bind(server, (struct sockaddr*)&localaddr, sizeof(localaddr)) == SOCKET_ERROR) //bind - запускает сервер (сокет)
	{
		printf("Can't start server\n");
		return 2;
	}
	else
	{
		printf("Server is started\n");
	}
	listen(server, 50);		//50 клиентов в очереди могут стоять, заставляет сервер слушать
	pthread_mutex_init(&mutex, NULL);
	pthread_mutex_init(&mutex_file, NULL);
	printf("Waiting for connection...\n");
	
	size = sizeof(clientaddr);

	pthread_t mythread2;
	int status2 = pthread_create(&mythread2, NULL, CheckMessage, (void*)client);

	while (client[nclients] = accept(server, (struct sockaddr*)&clientaddr, &size))
	{
		nclients++;
		printf("Client is accepted, total online: %d\n", nclients);
		pthread_t mythread;
		
		int status = pthread_create(&mythread, NULL, ClientStart, (void*)client[nclients - 1]);
		
		//pthread_detach(mythread);
	}

	pthread_mutex_destroy(&mutex_file);
	pthread_mutex_destroy(&mutex);
	printf("Server is stopped\n");
	closesocket(server);
	return 0;
}

int main()
{
	WSADATA wsd;
	if (WSAStartup(MAKEWORD(1, 1), &wsd) == 0)
	{
		printf("Connected to socket lib\n");
	}
	else
	{
		return 1;
	}
	return CreateServer();
}