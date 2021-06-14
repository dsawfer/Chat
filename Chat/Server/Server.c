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
char chat_members[100][100];
char chat_names[100][30];
char history[100][100];

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

void send_message_in_close_chat(char* message, int* membersID, int uid) {
	pthread_mutex_lock(&mutex);
	pthread_mutex_lock(&mutex_file);
	int step = 0;

	while (membersID[step] != 0)
	{
		for (int i = 0; i < 50; i++)
		{
			if (clients[i]) {
				if (clients[i]->uid == membersID[step] && clients[i]->uid != uid) {
					send(clients[i]->sockfd, message, strlen(message), 0);
				}
			}
		}
		step++;
	}

	pthread_mutex_unlock(&mutex_file);
	pthread_mutex_unlock(&mutex);
}

int getCommand(char* buff, char* command)
{
	int step = 0;
	while (buff[step] != ' ' && buff[step] != '\0') {
		command[step] = buff[step];
		step++;
	}
	if (buff[step] != '\0')
	{
		int point = 0;
		while (step >= 0) {
			while (buff[point]) {
				buff[point] = buff[point + 1];
				point++;
			}
			step--;
			point = 0;
		}
	}

	//printf("GetCommand: %s", command);

	if (strcmp(command, "/add") == 0) return 1;
	if (strcmp(command, "/create") == 0) return 2;
	if (strcmp(command, "/exit") == 0) return 3;
	if (strcmp(command, "/close_server") == 0) return 4;
	if (strcmp(command, "/delete_friend") == 0) return 5;
	if (strcmp(command, "/send") == 0) return 6;
	if (strcmp(command, "/history") == 0) return 7;
	return 0;
}

process_command(char* buff, char* title, char chat_members_args[][30])
{
	memset(title, '\0', 10);
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 30; j++)
		{
			chat_members_args[i][j] = '\0';
		}
	}

	int step = 0, point = 0, count = 0;
	while (buff[step] != ' ' && buff[step] != '\0') {
		title[step] = buff[step];
		step++;
	}

	step++;
	while (buff[step]) {
		while (buff[step] != ' ') {
			if (buff[step] == '\0') break;
			chat_members_args[count][point] = buff[step];
			step++;
			point++;
		}
		count++;
		point = 0;
		step++;
	}
}

process_message(char* buff, char* title, char* chat_message)
{
	memset(title, '\0', 10);
	memset(chat_message, '\0', 1024);

	int step = 0, point = 0;
	while (buff[step] != ' ' && buff[step] != '\0') {
		title[step] = buff[step];
		step++;
	}
	step++;
	while (buff[step]) {
		if (buff[step] == '\0') break;
		chat_message[point] = buff[step];
		//printf("%s %d %c\n", chat_message, point, buff[step]);
		step++;
		point++;
	}
}

void* ClientStart(void* client_socket)
{
	char name[30];
	char pass[30];
	client_t* cli = (client_t*)client_socket;
	char buff[1024];
	char message[1024];
	char command[20];
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
			printf("1\n");
			addUser(name, pass);
			printf("2\n");
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

	memset(buff, '\0', 1024);
	memset(command, '\0', 20);

	while (1) {

		if (leave_flag) {
			break;
		}

		ret = recv(cli->sockfd, buff, sizeof(buff), 0);
		if (strlen(buff) <= 0) continue;
		if (buff[0] == '/') {
			int comm = getCommand(buff, command);

			//printf("|%s| |%s| |%d|\n", buff, command, comm);

			switch (comm)
			{
			case 1:
				switch (findFriend(cli->name, buff))
				{
				case 0:
					printf("~%s wants to add a friend who has already been added\n", cli->name);
					memset(buff, '\0', 1024);
					sprintf(buff, "This user is already your friend");
					send(cli->sockfd, buff, sizeof(buff), 0);
					break;
				case -1:
					printf("~%s wants to add a non-existent user\n", cli->name);
					memset(buff, '\0', 1024);
					sprintf(buff, "This user does not exist");
					send(cli->sockfd, buff, sizeof(buff), 0);
					break;
				case 1:
					addFriend(buff, cli->name);
					sprintf(buff, "%s added a new friend", cli->name);
					printf("~%s\n", buff);
					send_message(buff, cli->uid);
					memset(buff, '\0', 1024);
					sprintf(buff, "New friend added");
					send(cli->sockfd, buff, sizeof(buff), 0);
					break;
				}
				break;
			case 2:
			{
				char title[10];
				char chat_members_args[10][30];
				process_command(buff, title, chat_members_args);

				printf("Creation of close chat %s with ", title);
				int step = 0;
				while (chat_members_args[step][0]) {
					printf("%s ", chat_members_args[step++]);
				}

				printf("\n");
				memset(buff, '\0', 1024);
				if (check_members(cli->name, chat_members_args) == 0) {
					if (findChat(title) == 0) {
						addChat(title, chat_members_args, cli->name);
						printf("Chat was created\n");
						sprintf(buff, "Chat was created");
						send(cli->sockfd, buff, sizeof(buff), 0);
					}
					else {
						printf("Chat already exist\n");
						sprintf(buff, "Chat already exist");
						send(cli->sockfd, buff, sizeof(buff), 0);
					}
				}
				else {
					printf("Few of members isn's in friend list\n");
					sprintf(buff, "Few of members isn's in friend list");
					send(cli->sockfd, buff, sizeof(buff), 0);
				}
			}
			break;
			case 3:
				sprintf(buff, "%s has left", cli->name);
				printf("~%s\n", buff);
				send_message(buff, cli->uid);
				leave_flag = 1;
				break;
			case 4:
				sprintf(buff, "Server was stoped by %s", cli->name);
				send_message(buff, cli->uid);
				server_flag = 1;
				break;
			case 5:
				switch (findFriend(cli->name, buff))
				{
				case 1:
					printf("~%s wants to delete a user who is not his friend\n", cli->name);
					sprintf(buff, "This user is not your friend, you cannot delete him");
					send(cli->sockfd, buff, sizeof(buff), 0);
					break;
				case -1:
					printf("~%s wants to delete a non-existent user\n", cli->name);
					sprintf(buff, "This user does not exist, you cannot delete him");
					send(cli->sockfd, buff, sizeof(buff), 0);
					break;
				case 0:
					delFriend(buff, cli->name);
					sprintf(buff, "%s deleted friend", cli->name);
					printf("~%s\n", buff);
					send_message(buff, cli->uid);
					memset(buff, '\0', 1024);
					sprintf(buff, "You deleted a friend");
					send(cli->sockfd, buff, sizeof(buff), 0);
					break;
				}
				break;
			case 6:
			{
				char title[10];
				char chat_message[1024];
				process_message(buff, title, chat_message);
				//printf("%s / %s\n", title, chat_message);
				int chat = findChat(title);
				if (chat) {
					int membersID[10] = { 0 };
					if (findMembers(chat, membersID, cli->name)) {
						printf("Member error\n");
						memset(buff, '\0', 1024);
						sprintf(buff, "You are not a member");
						send(cli->sockfd, buff, sizeof(buff), 0);
						break;
					}
					sprintf(message, "%s>%s: %s", title, cli->name, chat_message);
					printf("%s\n", message);
					send_message_in_close_chat(message, membersID, cli->uid);
				}
				else {
					printf("Chat does not exist\n");
					sprintf(buff, "Chat does not exist");
					send(cli->sockfd, buff, sizeof(buff), 0);
				}
			}
			break;
			case 7:
			{
				int i = 0;
				printf("~%s wants to see history\nHistory:\n", cli->name);
				while (history[i][0])
				{
					printf("%s\n", history[i]);
					send(cli->sockfd, history[i], sizeof(history[i]), 0);
					i++;
				}
			}
			break;
			}
		}
		else {
			sprintf(message, "%s: %s", cli->name, buff);
			send_message(message, cli->uid);
			printf(">%s\n", message);
			addToHistory(message);
		}

		memset(buff, '\0', 1024);
		memset(command, '\0', 20);
	}
	closesocket(cli->sockfd);
	removeClient(cli->uid);
	free(cli);
	nclients--;
	printf("-disconnect\n"); PRINTNUSERS
		closesocket(cli->sockfd);
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
	localaddr.sin_port = htons(11000);		//port number is for example, must be more than 1024 (5510)
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