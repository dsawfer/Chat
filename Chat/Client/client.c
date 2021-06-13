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

int authorization(SOCKET client)
{
	char login[30] = { 0 };
	char password[30] = { 0 };
	char buff[1024] = { 0 };

	printf("Login: ");

	gets_s(login, 30);
	if (!strlen(login) || strlen(login) > 30) {
		printf("Incorrect name, connection close\n");
		return -1;
	}
	send(client, login, sizeof(login), 0);		// (1) sending login
	int ret = recv(client, buff, sizeof(buff), 0);	// (2) getting answer
	printf("%s: ", buff);
	memset(buff, '\0', 1024);

	gets_s(password, 30);
	if (!strlen(password) || strlen(password) > 30) {
		printf("Incorrect password, connection close\n");
		return -1;
	}
	send(client, password, sizeof(password), 0);	// (3) sending password

	ret = recv(client, buff, sizeof(buff), 0);	// (4) getting answer
	if (strcmp(buff, "error") == 0) {
		printf("Login denied\n");
		return -1;
	}
	else {
		printf("Login successful\n");
		return 0;
	}
}

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
	//printf("1\n");
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

	char message[1024];
	int ret = 0;

	while (ret != SOCKET_ERROR) {
		gets_s(message, 1024);
		send(my_socket, message, sizeof(message), 0);

		if (!strcmp(message, "/exit"))
		{
			break;
		}
		memset(message, '\0', 1024);
	}
	flag = 1;
}

void CreateClient()
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
	server.sin_port = htons(5510);							//the same as in server
	server.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");	//special look-up address
	if (connect(client, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("Can't connect to server\n");
		closesocket(client);
		return;
	}
	printf("Connection established\n");
	
	if (authorization(client) == -1) return;		//authorization

	pthread_mutex_init(&mutex, NULL);
	pthread_mutex_init(&mutex_file, NULL);
	pthread_t getMessages;		//stream for checking new messages
	int getStatus = pthread_create(&getMessages, NULL, GetNewMassages, (void*)client);

	pthread_t sendMessages;		//stream for sending messages
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