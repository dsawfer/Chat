#pragma once
#include <winsock.h>
#define MAX_CLIENTS 50

//char message[1024] = "//message";
int nclients = 0;
int uid = 10;

typedef struct {
	struct sockaddr_in address;
	int sockfd;
	int uid;
	char name[32];
} client_t;

client_t* clients[MAX_CLIENTS];