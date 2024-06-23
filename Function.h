#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <winsock2.h>
#include <thread>
#include <string>
#include <conio.h>
#include "ws2tcpip.h"
#include <WS2tcpip.h>
using namespace std;
int USER(char* login)
{
	char buf[50];

	ifstream userInfo("password.txt");

	while (!userInfo.eof()) {
		userInfo >> buf;

		if (!strcmp(login, buf)) {
			userInfo.close();
			return 1;
		}
		for (int i = 0; i < 2; i++)
			userInfo >> buf;
	}

	if (userInfo.eof()) {
		userInfo.close();
		return 0;
	}
	else
		return 0;
}

int PASS(char* login, char* password)
{
	char buf[50];

	ifstream userInfo("password.txt");

	while (!userInfo.eof()) {
		userInfo >> buf;

		if (!strcmp(login, buf))
			break;
		else
			for (int i = 0; i < 2; i++)
				userInfo >> buf;
	}

	for (int i = 0; i < 2; i++)
		userInfo >> buf;

	if (!strcmp(password, buf)) {
		userInfo.close();
		return 1;
	}
	else {
		userInfo.close();
		return 0;
	}

	if (userInfo.eof()) {
		userInfo.close();
		return 0;
	}
}

int AUTH(char* login, char* password) {
	string str;
	string user = (string)login + " : " + (string)password;
	ifstream userInfo("password.txt");

	while (!userInfo.eof()) {
		getline(userInfo, str);
		if (str == user)
			return 1;
	}

	if (userInfo.eof()) {
		userInfo.close();
		return 0;
	}
}

void STAT(char* login, int* a) { //вернет массив 2 элементов, нулевой кол-во писем, первый это объем
	a[0] = 0;
	a[1] = 0;
	string str;

	char txt_name[50];
	strcpy_s(txt_name, login);
	strcat_s(txt_name, ".txt");

	ifstream mail(txt_name);

	getline(mail, str);
	while (!mail.eof()) {
		while (!mail.eof() && (str != ".")) {
			a[1] += str.length();
			getline(mail, str);
		}

		a[0]++;
		getline(mail, str);
	}

	mail.close();
}

int* LIST(char* login, int* b) { //вернет массив длины = кол-во писем + 1, в нулевом общая длина, в итом длина итого сообщения
	int len = 1;
	int* a = (int*)malloc(len);
	a[0] = 0;
	string str;
	int k = 0, s = 0;

	char txt_name[50];
	strcpy_s(txt_name, login);
	strcat_s(txt_name, ".txt");

	ifstream mail(txt_name);

	getline(mail, str);
	while (!mail.eof()) {
		while (!mail.eof() && (str != ".")) {
			k += str.length();
			getline(mail, str);
		}
		len++;
		a = (int*)realloc(a, len * sizeof(int));
		a[len - 1] = k;
		s += k;
		k = 0;
		getline(mail, str);
	}
	a[0] = s;
	len++;
	a = (int*)realloc(a, len * sizeof(int));
	a[len - 1] = 0;
	b = a;
	mail.close();
	return b;
}

string RETR(char* login, int i) {

	string str = "";
	string s = "";
	char buf[50] = "";
	int k = i;
	char txt_name[50];
	strcpy_s(txt_name, login);
	strcat_s(txt_name, ".txt");

	ifstream mail(txt_name);


	while (k - 1 > 0) {
		while (!mail.eof() && strcmp(buf, "."))
			mail >> buf;
		k--;
		mail >> buf;
	}

	str += (string)buf;
	while (!mail.eof() && (s != ".")) {
		getline(mail, s);
		if (s != ".")
			str += s + "\r\n";
	}

	mail.close();
	return str;
}

