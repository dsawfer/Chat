#pragma once
#include <winsock.h>

char message[1024] = "//message";
int nclients = 0;

void* ClientStart(void* param);
int CreateServer();
//void* proccesMessage(void* param);
int proccesMessage(SOCKET clients[], char message[], int point, int curClt);
void checkMessages(SOCKET clients[], int point);