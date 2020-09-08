#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <stdio.h>
#include <winsock2.h>
#include "filesend.h"
#pragma comment(lib,"ws2_32.lib")

#define  MAX_DATA_BLOCK_SIZE 8192

void send_file(const char* file_name, const char* ip, u_short port) {

	WSADATA wsaData;
	SOCKET s;
	FILE *fp;
	struct sockaddr_in server_addr;
	char data[MAX_DATA_BLOCK_SIZE];
	int i;
	int ret;
	fp = fopen(file_name, "rb");
	if (fp == NULL) {
		printf("�޷����ļ�\n");
		return;
	}
	WSAStartup(0x202, &wsaData);
	s = socket(AF_INET, SOCK_STREAM, 0);
	memset((void *)&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ip);
	server_addr.sin_port = htons(port);
	if (connect(s, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in)) == SOCKET_ERROR) {
		printf("���ӷ�����ʧ��\n");
		fclose(fp);
		closesocket(s);
		WSACleanup();
		return;
	}
	printf("�����ļ���������\n");
	send(s, file_name, strlen(file_name), 0);
	send(s, "\0", 1, 0);
	printf("�����ļ�����");
	for (;;) {
		memset((void *)data, 0, sizeof(data));
		i = fread(data, 1, sizeof(data), fp);
		if (i == 0) {
			printf("\n���ͳɹ�\n");
			break;
		}
		ret = send(s, data, i, 0);
		putchar('.');
		if (ret == SOCKET_ERROR) {
			printf("\n����ʧ�ܣ��ļ����ܲ�����\n");
			break;
		}
	}
	fclose(fp);
	closesocket(s);
	WSACleanup();
}