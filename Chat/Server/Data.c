#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include "ServerParams.h"

#define width 30		//width of logins and passwords
#define length 100		//max amount of logins and passwords

extern char logins[100][30];
extern char passwords[100][30];

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
}

int findLogin(char *name)
{
	int step = 0;
	while (logins[step][0]) {
		if (strcmp(logins[step], name) == 0)
			return step;
		step++;
	}
	return -1;
}

load()
{
	FILE* l_logins = fopen("Data/logins.txt", "r+");
	FILE* l_passwords = fopen("Data/passwords.txt", "r+");

	for (int i = 0; i < length; i++)
	{
		for (int j = 0; j < width; j++)
		{
			logins[i][j] = '\0';
			passwords[i][j] = '\0';
		}
	}

	int step = 0, len = 0;
	char tempStr[30] = { 0 };
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
		step++;
	}

	fclose(l_logins);
	fclose(l_passwords);
}

save()
{
	FILE* l_logins = fopen("Data/logins.txt", "r+");
	FILE* l_passwords = fopen("Data/passwords.txt", "r+");

	int step = 0;
	while (logins[step][0])
	{
		fprintf(l_logins, "%s\n", logins[step]);
		fprintf(l_passwords, "%s\n", passwords[step]);
		step++;
	}

	fclose(l_logins);
	fclose(l_passwords);
}