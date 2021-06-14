#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include "ServerParams.h"

#define width 30		//width of logins and passwords
#define width_fr 100    //width of friends
#define length 100		//max amount of logins and passwords

extern char logins[100][30];
extern char passwords[100][30];
extern char friends[100][100];
extern char chat_members[100][100];
extern char chat_names[100][30];

process_command(char* buff, char* title, char chat_members_args[][30])
{
	//printf("2\n");
	memset(title, 10, '\0');
	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 30; j++)
		{
			chat_members_args[i][j] = '\0';
		}
	}
	//printf("3\n");
	int step = 0, point = 0, count = 0;
	while (buff[step] != ' ' && buff[step] != '\0') {
		//printf("3.5: %s\n", title);
		title[step] = buff[step];
		step++;
	}
	//printf("4: %s\n", title);
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

int findPassword(int step, char pass[])
{
	if (strcmp(passwords[step], pass) == 0) return 0;
	else return -1;
}

void addUser(char name[], char pass[])
{
	int step = 0;
	while (logins[step][0]) {
		step++;
	}
	strcpy(logins[step], name);
	strcpy(passwords[step], pass);
	strcpy(friends[step], "\n");
}

int findLogin(char* name)
{
	int step = 0;
	while (logins[step][0]) {
		if (strcmp(logins[step], name) == 0)
			return step;
		step++;
	}
	return -1;
}

int findFriend(char* name, char* buff)
{
	int n = 0;
	n = findLogin(name);
	char temp_fr_name[30] = { 0 };
	int t = 0;
	for (int i = 0; i < width_fr; i++)
	{
		if (friends[n][i] != ' ')
		{
			temp_fr_name[t] = friends[n][i];
			t++;
		}
		else
		{
			if (strcmp(temp_fr_name, buff) == 0)
				return 0; //this user is alredy your friend
			else
			{
				for (t = 0; t < width; t++)
					temp_fr_name[t] = '\0';
				t = 0;
			}
		}
	}
	int f = 0;
	f = findLogin(buff);

	printf("\n%s - %s\n\n", temp_fr_name, buff);

	strcpy(temp_fr_name, buff);
	if (f > -1)
	{
		return 1;      //you can add friend
	}
	return -1;         //user doesn't exist
}

void addFriend(char* buff, char* userName)
{
	int n = 0;
	n = findLogin(userName);
	strcat(friends[n], buff);
	strcat(friends[n], " ");
}

load()
{
	FILE* l_logins = fopen("Data/logins.txt", "r+");
	FILE* l_passwords = fopen("Data/passwords.txt", "r+");
	FILE* l_friends = fopen("Data/friends.txt", "r+");
	FILE* l_chat_members = fopen("Data/chat_members.txt", "r+");
	FILE* l_chat_names = fopen("Data/chat_names.txt", "r+");

	for (int i = 0; i < length; i++)
	{
		for (int j = 0; j < width; j++)
		{
			logins[i][j] = '\0';
			passwords[i][j] = '\0';
			chat_names[i][j] = '\0';
		}
		for (int j = 0; j < width_fr; j++)
		{
			friends[i][j] = '\0';
			chat_members[i][j] = '\0';
		}
	}

	int step = 0, len = 0;
	char tempStr[30] = { 0 };
	char tempStr_long[100] = { 0 };
	while (!feof(l_logins))
	{
		fgets(tempStr, 30, l_logins);
		len = (strlen(tempStr) - 1);
		if (tempStr[len] == '\n')
			tempStr[len] = '\0';
		strcpy(logins[step], tempStr);

		fgets(tempStr, 30, l_passwords);
		len = (strlen(tempStr) - 1);
		if (tempStr[len] == '\n')
			tempStr[len] = '\0';
		strcpy(passwords[step], tempStr);

		fgets(tempStr_long, 100, l_friends);
		len = (strlen(tempStr_long) - 1);
		if (tempStr_long[len] == '\n')
			tempStr_long[len] = '\0';
		strcpy(friends[step], tempStr_long);
		step++;
	}

	memset(logins[step - 1], '\0', 30);
	memset(passwords[step - 1], '\0', 30);
	memset(friends[step - 1], '\0', 100);

	step = 0;
	while (!feof(l_chat_names))
	{
		fgets(tempStr_long, 100, l_chat_members);
		len = (strlen(tempStr_long) - 1);
		if (tempStr_long[len] == '\n')
			tempStr_long[len] = '\0';
		strcpy(chat_members[step], tempStr_long);

		fgets(tempStr, 30, l_chat_names);
		len = (strlen(tempStr) - 1);
		if (tempStr[len] == '\n')
			tempStr[len] = '\0';
		strcpy(chat_names[step], tempStr);
		step++;
	}

	memset(chat_members[step - 1], '\0', 100);
	memset(chat_names[step - 1], '\0', 30);

	fclose(l_logins);
	fclose(l_passwords);
	fclose(l_friends);
	fclose(l_chat_members);
	fclose(l_chat_names);
}

save()
{
	FILE* l_logins = fopen("Data/logins.txt", "r+");
	FILE* l_passwords = fopen("Data/passwords.txt", "r+");
	FILE* l_friends = fopen("Data/friends.txt", "r+");
	FILE* l_chat_members = fopen("Data/chat_members.txt", "r+");
	FILE* l_chat_names = fopen("Data/chat_names.txt", "r+");

	int step = 0;
	while (logins[step][0])
	{
		fprintf(l_logins, "%s\n", logins[step]);
		fprintf(l_passwords, "%s\n", passwords[step]);
		fprintf(l_friends, "%s\n", friends[step]);
		step++;
	}

	step = 0;
	while (chat_names[step][0])
	{
		fprintf(l_chat_members, "%s\n", chat_members[step]);
		fprintf(l_chat_names, "%s\n", chat_names[step]);
		step++;
	}

	fclose(l_logins);
	fclose(l_passwords);
	fclose(l_friends);
	fclose(l_chat_members);
	fclose(l_chat_names);
}