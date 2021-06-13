// server.cpp : Ýòîò ôàéë ñîäåðæèò ôóíêöèþ "main". Çäåñü íà÷èíàåòñÿ è çàêàí÷èâàåòñÿ âûïîëíåíèå ïðîãðàììû.
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
#include "ServerParams.h"

//#define clientHello "Hello, new member! This's our win socket chat!))\r\n"

#define PRINTNUSERS if (nclients)\
  printf("%d users online\n",nclients);\
  else printf("No User online\n");

pthread_mutex_t mutex;
pthread_mutex_t mutex_file;

char logins[100][30];
char passwords[100][30];
char friends[100][100];
int nclients = 0;
int uid = 10;
server_flag = 0;

void addClient(client_t* cl)
{
	pthread_mutex_lock(&mutex);
	pthread_mutex_lock(&mutex_file);

	for (int i = 0; i < 50; ++i) {
		if (!clients[i]) {
			clients[i] = cl;
			break;
		}
	}

	pthread_mutex_unlock(&mutex_file);
	pthread_mutex_unlock(&mutex);
}

void removeClient(int uid) {
	pthread_mutex_lock(&mutex);
	pthread_mutex_lock(&mutex_file);

	for (int i = 0; i < 50; ++i) {
		if (clients[i]) {
			if (clients[i]->uid == uid) {
				clients[i] = NULL;
				break;
			}
		}
	}

	pthread_mutex_unlock(&mutex_file);
	pthread_mutex_unlock(&mutex);
}


void send_message(char* message, int uid) {
	pthread_mutex_lock(&mutex);
	pthread_mutex_lock(&mutex_file);

	for (int i = 0; i < 50; i++)
	{
		if (clients[i]) {
			if (clients[i]->uid != uid) {
				send(clients[i]->sockfd, message, strlen(message), 0);
			}
		}
	}

	pthread_mutex_unlock(&mutex_file);
	pthread_mutex_unlock(&mutex);
}

void* ClientStart(void* client_socket)
{
	char name[30];
	char pass[30];
	client_t* cli = (client_t*)client_socket;
	char buff[1024];
	char message[1024];
	int leave_flag = 0, res = 0;

	int ret = recv(cli->sockfd, name, sizeof(name), 0);		// (1) get login

	if (ret == 0 || ret == SOCKET_ERROR) {
		printf("Didn't enter the name.\n");
		leave_flag = 1;
	}
	else {
		strcpy(cli->name, name);
		res = findLogin(name);

		if (res == -1) {
			printf("Creation of a new user: %s\n", name);
			sprintf(buff, "New user, please, create the password");
			send(cli->sockfd, buff, sizeof(buff), 0);	// (2) sending answer
			memset(buff, '\0', 1024);

			ret = recv(cli->sockfd, pass, sizeof(pass), 0);		// (3) getting password
			addUser(name, pass);

			sprintf(buff, "success");
			send(cli->sockfd, buff, sizeof(buff), 0);		// (4) sending answer
		}
		else {
			printf("Login of an existing user: %s\n", name);
			sprintf(buff, "Enter the password");
			send(cli->sockfd, buff, sizeof(buff), 0);	// (2) sending answer
			memset(buff, '\0', 1024);

			printf("Password is %s\n", passwords[res]);

			ret = recv(cli->sockfd, pass, sizeof(pass), 0);		// (3) getting password

			if (findPassword(res, pass) == -1) {
				printf("Wrong password\n");
				sprintf(buff, "error");
				send(cli->sockfd, buff, sizeof(buff), 0);		// (4) sending answer
				leave_flag = 1;
			}
			else {
				printf("Correct password\n");
				sprintf(buff, "success");
				send(cli->sockfd, buff, sizeof(buff), 0);		// (4) sending answer
			}
		}
		memset(buff, '\0', 1024);
		printf("Client %s is accepted\n", cli->name);
		sprintf(buff, "%s has joined", cli->name);
		send_message(buff, cli->uid);
	}
	//
	memset(buff, '\0', 1024);

	while (1) {

		if (leave_flag) {
			break;
		}

		ret = recv(cli->sockfd, buff, sizeof(buff), 0);
		if (ret > 0) {
			if (strlen(buff) > 0) {
				if (strcmp(buff, "/exit") == 0) {
					sprintf(buff, "%s has left", cli->name);
					printf("~%s\n", buff);
					send_message(buff, cli->uid);
					leave_flag = 1;
				}
				else if (buff[0] == '/' && buff[1] == 'a' && buff[2] == 'd' && buff[3] == 'd') {
					int f = findFriend(cli->name, buff);
					if (f == 0)
					{
						printf("~%s wants to add a friend who has already been added\n", cli->name);

						memset(buff, '\0', 1024);
						sprintf(buff, "This user is already your friend");
						send(cli->sockfd, buff, sizeof(buff), 0);

						memset(buff, '\0', 1024);
					}
					else if (f == -1)
					{
						printf("~%s wants to add a non-existent user\n", cli->name);
						memset(buff, '\0', 1024);
						sprintf(buff, "This user does not exist");
						send(cli->sockfd, buff, sizeof(buff), 0);
						memset(buff, '\0', 1024);
					}
					else
					{
						addFriend(buff, cli->name);
						memset(buff, '\0', 1024);
						sprintf(buff, "%s added a new friend", cli->name);
						printf("~%s\n", buff);
						send_message(buff, cli->uid);
						memset(buff, '\0', 1024);
						sprintf(buff, "New friend added");
						send(cli->sockfd, buff, sizeof(buff), 0);
						memset(buff, '\0', 1024);
					}
				}
				else if (strcmp(buff, "/close_server") == 0) {
					sprintf(buff, "Server was stoped by %s", cli->name);
					send_message(buff, cli->uid);
					server_flag = 1;
				}
				else {
					sprintf(message, "%s: %s", cli->name, buff);
					send_message(message, cli->uid);
					//str_trim_lf(buff, strlen(buff));
					printf(">%s\n", message);
					//addHistory();
				}
			}
		}
		memset(buff, '\0', 1024);
	}
	closesocket(cli->sockfd);
	removeClient(cli->uid);
	free(cli);
	nclients--;
	printf("-disconnect\n"); PRINTNUSERS
		//closesocket(my_socket);
		pthread_detach(pthread_self());
	return NULL;
}

int CreateServer()
{
	SOCKET server;
	SOCKET client;
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
	if (bind(server, (struct sockaddr*)&localaddr, sizeof(localaddr)) == SOCKET_ERROR) //bind - çàïóñêàåò ñåðâåð (ñîêåò)
	{
		printf("Can't start server\n");
		return 2;
	}
	else
	{
		printf("Server is started\n");
	}
	listen(server, 50);		//50 êëèåíòîâ â î÷åðåäè ìîãóò ñòîÿòü, çàñòàâëÿåò ñåðâåð ñëóøàòü
	pthread_mutex_init(&mutex, NULL);
	pthread_mutex_init(&mutex_file, NULL);
	printf("Waiting for connection...\n");

	size = sizeof(clientaddr);

	load();		//load data base

	while ((client = accept(server, (struct sockaddr*)&clientaddr, &size)) && server_flag == 0)
	{
		printf("Client join\n");
		nclients++;

		client_t* cli = (client_t*)malloc(sizeof(client_t));
		cli->address = clientaddr;
		cli->sockfd = client;
		cli->uid = uid++;

		addClient(cli);

		pthread_t mythread;
		int status = pthread_create(&mythread, NULL, ClientStart, (void*)cli);

		//pthread_detach(mythread);
	}

	save();		//save data base

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