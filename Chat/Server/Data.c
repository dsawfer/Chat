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
	strcpy(friends[step], " ");
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
}

void delFriend(char* buff, char* userName)
{
	int n = 0;
	n = findLogin(userName);
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
			{
				i--;
				int k = 0;
				while (friends[n][i] != ' ')
				{
					i--;
					k++;
				}
				i++;
				for (i; i < width_fr - k; i++)
					friends[n][i] = friends[n][i + k];
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

load()
{
	FILE* l_logins = fopen("Data/logins.txt", "r+");
	FILE* l_passwords = fopen("Data/passwords.txt", "r+");
	FILE* l_friends = fopen("Data/friends.txt", "r+");

	for (int i = 0; i < length; i++)
	{
		for (int j = 0; j < width; j++)
		{
			logins[i][j] = '\0';
			passwords[i][j] = '\0';
		}
		for (int j = 0; j < width_fr; j++)
		{
			friends[i][j] = '\0';
		}
	}

	int step = 0, len = 0;
	char tempStr[30] = { 0 };
	char tempStr_fr[100] = { 0 };
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

		fgets(tempStr_fr, 100, l_friends);
		len = (strlen(tempStr_fr) - 1);
		if (tempStr_fr[len] == '\n')
			tempStr_fr[len] = '\0';
		strcpy(friends[step], tempStr_fr);
		step++;
	}

	fclose(l_logins);
	fclose(l_passwords);
	fclose(l_friends);
}

save()
{
	FILE* l_logins = fopen("Data/logins.txt", "r+");
	FILE* l_passwords = fopen("Data/passwords.txt", "r+");
	FILE* l_friends = fopen("Data/friends.txt", "r+");

	int step = 0;
	while (logins[step][0])
	{
		fprintf(l_logins, "%s\n", logins[step]);
		fprintf(l_passwords, "%s\n", passwords[step]);
		fprintf(l_friends, "%s\n", friends[step]);
		step++;
	}

	fclose(l_logins);
	fclose(l_passwords);
	fclose(l_friends);
}