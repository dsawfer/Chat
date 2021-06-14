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
extern char history[100][100];

int findMemberID(char member[])
{
	for (int i = 0; i < 50; i++) {
		if (clients[i]) {
			//printf("%s %s\n", clients[i]->name, member);
			if (strcmp(clients[i]->name, member) == 0) {
				return clients[i]->uid;
			}
		}
	}
	return 0;
}

int findMembers(int chat, int membersID[], char* creator)
{
	char member[30] = { 0 };
	memset(member, '\0', 30);
	int checker = 0;

	int step = 0, point = 0, count = 0, ID = 0;

	//printf("%s\n", chat_members[chat]);
	while (chat_members[chat][step]) {
		while (chat_members[chat][step] != ' ') {
			if (chat_members[chat][step] == '\0') break;
			member[point] = chat_members[chat][step];
			step++;
			point++;
		}
		//printf("%s\n",member);
		ID = findMemberID(member);
		//printf("1\n");
		if (ID != 0) membersID[count++] = ID;

		if (strcmp(creator, member) == 0) checker++;

		memset(member, '\0', 30);
		point = 0;
		step++;
	}
	if (checker) return 0;
	else return 1;
}

int check_members(char name[], char chat_members_args[][30])
{
	int step = 0;
	while (chat_members_args[step][0]) {
		//printf("%s %s\n", name, chat_members_args[step]);
		if (findFriend(name, chat_members_args[step]) != 0)
			return 1;
		step++;
	}
	return 0;
}

int findChat(char title[])
{
	int step = 0;
	while (chat_names[step][0])
	{
		//printf("%d %s %s\n", step, title, chat_names[step]);
		if (strcmp(title, chat_names[step]) == 0) return step;
		step++;
	}
	return -1;
}

void addChat(char title[], char chat_members_args[][30], char* creator)
{
	int step = 0, point = 0;
	while (chat_names[step][0]) {
		step++;
	}
	strcpy(chat_names[step], title);

	while (chat_members_args[point][0]) {
		strcat(chat_members[step], chat_members_args[point]);
		strcat(chat_members[step], " ");
		point++;
	}
	strcat(chat_members[step], creator);
	strcat(chat_members[step], " ");
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
	friends[step][0] = '0';
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
	int n = 0, step = 0;
	n = findLogin(name);
	char temp_fr_name[30] = { 0 };
	int t = 0;
	while (friends[n][step])
	{
		if (friends[n][step] == '0') {
			friends[n][step] = '\0';
			break;
		}
		if (friends[n][step] != ' ')
		{
			temp_fr_name[t] = friends[n][step];
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
		step++;
	}

	if (strcmp(temp_fr_name, buff) == 0)
		return 0; //this user is alredy your friend

	int f = 0;
	f = findLogin(buff);
	if (f > -1)
		return 1;      //you can add friend
	return -1;         //user doesn't exist
}

void addFriend(char* buff, char* userName)
{
	int n = 0;
	n = findLogin(userName);
	strcat(friends[n], buff);
	strcat(friends[n], " ");

	printf("%s\n", friends[n]);

}

void delFriend(char* buff, char* userName)
{
	int n = 0;
	n = findLogin(userName);
	char temp_fr_name[30] = { 0 };
	int t = 0;
	for (int i = 0; i < width_fr; i++)
	{
		if (friends[n][i] != ' ' && friends[n][i] != '\n' && friends[n][i] != '\0')
		{
			temp_fr_name[t] = friends[n][i];
			t++;
		}
		else
		{
			if (strcmp(temp_fr_name, buff) == 0)
			{
				i -= t;
				t++;
				for (i; i < width_fr - t; i++)
					friends[n][i] = friends[n][i + t];
				int c = 0;
				for (i = 0; friends[n][i] != '\0'; i++)
				{
					if (friends[n][i] != ' ')
					{
						c++;
					}
				}
				if (c == 0)
				{
					strcpy(friends[n], "0");
				}
				i = width_fr;
			}
			else
			{
				for (t = 0; t < width; t++)
					temp_fr_name[t] = '\0';
				t = 0;
			}
		}
	}
}

void addToHistory(char* message)
{
	int i = 0;
	while (history[i][0] != '\0')
		i++;
	strcpy(history[i], message);
}

load()
{
	FILE* l_logins = fopen("Data/logins.txt", "r+");
	FILE* l_passwords = fopen("Data/passwords.txt", "r+");
	FILE* l_friends = fopen("Data/friends.txt", "r+");
	FILE* l_chat_members = fopen("Data/chat_members.txt", "r+");
	FILE* l_chat_names = fopen("Data/chat_names.txt", "r+");
	FILE* l_history = fopen("Data/history.txt", "r+");

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
			history[i][j] = '\0';
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

	step = 0;
	while (!feof(l_history))
	{
		fgets(tempStr, 100, l_history);
		len = (strlen(tempStr) - 1);
		if (tempStr[len] == '\n')
			tempStr[len] = '\0';
		strcpy(history[step], tempStr);
		step++;
	}

	memset(history[step - 1], '\0', 100);

	fclose(l_logins);
	fclose(l_passwords);
	fclose(l_friends);
	fclose(l_chat_members);
	fclose(l_chat_names);
	fclose(l_history);
}

save()
{
	FILE* l_logins = fopen("Data/logins.txt", "r+");
	FILE* l_passwords = fopen("Data/passwords.txt", "r+");
	FILE* l_friends = fopen("Data/friends.txt", "wt");
	FILE* l_chat_members = fopen("Data/chat_members.txt", "r+");
	FILE* l_chat_names = fopen("Data/chat_names.txt", "r+");
	FILE* l_history = fopen("Data/history.txt", "r+");

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

	step = 0;
	while (history[step][0])
	{
		fprintf(l_history, "%s\n", history[step]);
		step++;
	}

	fclose(l_logins);
	fclose(l_passwords);
	fclose(l_friends);
	fclose(l_chat_members);
	fclose(l_chat_names);
	fclose(l_history);
}