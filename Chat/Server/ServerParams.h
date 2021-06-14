#pragma once
#include <winsock.h>
#define MAX_CLIENTS 50

//char message[1024] = "//message";


typedef struct {
	struct sockaddr_in address;
	int sockfd;
	int uid;
	char name[30];
} client_t;

client_t* clients[MAX_CLIENTS];

load();		//char** logs, char** pass
int findLogin(char* name);
void addUser(name, pass);
int findPassword(res, pass);
int findFriend(char* name, char* buff);
void addFriend(char* buff, char* userName);
process_command(char* buff, char* title, char chat_members_args[][30]);
int check_members(char name[], char chat_members_args[][30]);
int findChat(char title[]);
void addChat(char title[], char chat_members_args[][30], char* creator);
int findMembers(int chat, int membersID[], char* creator);
save();