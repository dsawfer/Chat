#pragma once
#include <winsock.h>
#define MAX_CLIENTS 50

//char message[1024] = "//message";


typedef struct {
	struct sockaddr_in address;
	int sockfd;
	int uid;
	char name[32];
} client_t;

client_t* clients[MAX_CLIENTS];

load();		//char** logs, char** pass
int findLogin(char* name);
void addUser(name, pass);
int findPassword(res, pass);
int findFriend(name, buf);
void addFriend(buf, userName);
void delFriend(buf, userName);
save();