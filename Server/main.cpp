#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <process.h>
#include <winsock2.h>

#pragma  comment(lib,"ws2_32.lib")

#define MAX_DATA_BLOCK_SIZE 8192
void error_exit(const char *msg, int val);
void serve_at(u_short port);
void serve_client(void *s);
void print_socket_detail(SOCKET s);

int main(int argc, char ** argv) {
	u_short port;
	if (argc == 1) {
		serve_at(8888);
	}
	else if (argc == 2) {
		port = (u_short)atoi(argv[1]);
		if (port == 0) {
			error_exit("�Ƿ��ļ����˿�", -1);
		}
		else {
			serve_at(port);
		}
	}
	else {
		error_exit("��������", -1);
	}
	return 0;
}

void error_exit(const char * msg, int val) {
	if (msg) { printf("%s\n\n", msg); }
	printf("ʹ�÷�����ft_server [�����˿�]\n");
	printf("�����˿��ǿ�ѡ������Ĭ��Ϊ8888\n\n");
	exit(val);

}
void serve_at(u_short port) {
	WSADATA wsaData;
	SOCKET ls, as;
	static SOCKET *a;
	struct sockaddr_in addr;
	struct sockaddr_in cli_addr;
	int cli_addr_len;
	WSAStartup(0x202, &wsaData);
	ls = socket(AF_INET, SOCK_STREAM, 0);
	memset((void *)&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr("0.0.0.0");
	addr.sin_port = htons(port);
	bind(ls, (struct sockaddr *)&addr, sizeof(addr));
	listen(ls, SOMAXCONN);
	printf("�������������������ڶ˿�%d\n", port);
	for (;;) {
		cli_addr_len = sizeof(cli_addr);
		memset((void *)&cli_addr, 0, cli_addr_len);
		as = accept(ls, (struct sockaddr *)&cli_addr, &cli_addr_len);
		a = &as;
		printf("�ͻ���%s:%d������\n", inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port));
		_beginthread(serve_client, 0, (void *)a);
		Sleep(1000);
		print_socket_detail(as);
		// while(1){}
	}
	closesocket(ls);
	WSACleanup();
}


void serve_client(void *s) {
	printf("\n\n\n�������̳߳ɹ���\n\n\n");
	char file_name[MAX_PATH];
	char data[MAX_DATA_BLOCK_SIZE];
	int i;
	char c;
	FILE *fp;
	printf("�����ļ�����������\n");
	memset((void *)file_name, 0, sizeof(file_name));
	for (i = 0; i < sizeof(file_name); i++) {
		if (recv(*(SOCKET *)s, &c, 1, 0) != 1) {
			printf("����ʧ�ܻ�ͻ����ѹر�����\n");
			closesocket(*(SOCKET *)s);
			return;
		}
		if (c == 0) {
			break;
		}
		file_name[i] = c;
	}
	if (i == sizeof(file_name)) {
		printf("�ļ�������\n");
		closesocket(*(SOCKET *)s);
		return;
	}
	printf("�ļ���%s\n", file_name);
	fp = fopen(file_name, "wb");
	if (fp == NULL) {
		printf("�޷���д��ʽ���ļ�\n");
		closesocket(*(SOCKET *)s);
		return;
	}
	printf("�����ļ�����");
	memset((void *)data, 0, sizeof(data));
	for (;;) {

		i = recv(*(SOCKET *)s, data, sizeof(data), 0);
		putchar('.');
		if (i == SOCKET_ERROR) {
			printf("\n����ʧ�ܣ��ļ����ܲ�����\n");
			break;
		}
		else if (i == 0) {
			printf("\n���ճɹ�\n");
			break;
		}
		else {
			fwrite((void *)data, 1, i, fp);
		}
	}
	printf("%s", data);
	fclose(fp); _endthread();
	closesocket(*(SOCKET *)s);

}

void print_socket_detail(SOCKET s) {
	struct sockaddr_in name;
	int namelen;
	namelen = sizeof(name);
	memset(&name, 0, namelen);
	getsockname(s, (struct sockaddr*)&name, &namelen);
	printf("local:%s:%d\n", inet_ntoa(name.sin_addr), ntohs(name.sin_port));
	namelen = sizeof(name);
	memset(&name, 0, namelen);
	getpeername(s, (struct sockaddr*)&name, &namelen);
	printf("peer:%s:%d\n", inet_ntoa(name.sin_addr), ntohs(name.sin_port));
}