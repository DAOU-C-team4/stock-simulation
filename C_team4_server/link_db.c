#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"
#include <openssl/sha.h> // OpenSSL ���̺귯�� ���
#include <WinSock2.h>    // ��Ʈ��ũ ���� ���� ���
#include <WS2tcpip.h>    // TCP/IP ����� ���� ���
#pragma comment(lib, "Ws2_32.lib") // Windows ���� ���̺귯�� ��ũ
#define PORT 8080
#define MAX_BUFFER_SIZE 1024
#define FD_SETSIZE 100

// �Լ� ����
static int callback(void* NotUsed, int argc, char** argv, char** azColName);
sqlite3* open_database(const char* db_name);
int create_member_table(sqlite3* db);
int insert_member(sqlite3* db, const char* name, int age, const char* email, const char* user_id, const char* password);
int open_socket();
DWORD WINAPI handle_client(LPVOID lpParam);

// ���� ���� ����
SOCKET client_sockets[FD_SETSIZE];
int num_clients = 0;

int main(int argc, char* argv[]) {
	// �����ͺ��̽� ����
	sqlite3* db = open_database("customer.db");
	if (!db) {
		return 1;
	}

	// ���� ���Ͽ���
	int server_fd, new_socket, valread;
	server_fd = open_socket();
	if (server_fd == -1) {
		fprintf(stderr, "Failed to open socket\n");
		return 1;
	}

	// Ŭ���̾�Ʈ�� ������ ��ٸ�
	while (1) {
		int new_socket = accept(server_fd, NULL, NULL);
		client_sockets[num_clients++] = new_socket;
		printf("\n");
		if (new_socket == -1) {
			perror("accept failed");
			free(new_socket);
			continue;
		}

		// Ŭ���̾�Ʈ�� ����Ǿ����� �˸�
		printf("%d�� Ŭ���̾�Ʈ ����\n", new_socket);

		// Ŭ���̾�Ʈ ��û�� ó���ϴ� ���ο� ������ ����
		DWORD dwThreadId;
		HANDLE hThread = CreateThread(NULL, 0, handle_client, new_socket, 0, &dwThreadId);
		if (hThread == NULL) {
			perror("Thread creation failed");
			free(new_socket);
			continue;
		}
	}

	closesocket(new_socket);


	///////// �����ڿ� //////////
	// ������ ���̺� ����
	//create_member_table(db);
	// �� ���� �߰�
	/*insert_member(db, "John Doe", 30, "john@example.com", "johndoe", "password123");
	insert_member(db, "Jane Smith", 25, NULL, "janesmith", "pass456");*/

	return 0;
}

// �ݹ� �Լ� - SELECT ���� ����� ó���ϴ� �Լ�
static int callback(void* NotUsed, int argc, char** argv, char** azColName) {
	int i;
	for (i = 0; i < argc; i++) {
		printf("%s: %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}

// �����ͺ��̽� ���� �Լ�
sqlite3* open_database(const char* db_name) {
	sqlite3* db;
	int rc = sqlite3_open(db_name, &db);
	if (rc) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		return NULL;
	}
	else {
		fprintf(stderr, "Opened database successfully\n");
		return db;
	}
}

// ȸ�� ���̺� ���� �Լ�
int create_member_table(sqlite3* db) {
	char* zErrMsg = 0;
	const char* sql_create_table =
		"CREATE TABLE MEMBER(" \
		"ID INTEGER PRIMARY KEY AUTOINCREMENT," \
		"NAME TEXT NOT NULL," \
		"AGE INTEGER," \
		"EMAIL TEXT," \
		"USER_ID TEXT UNIQUE NOT NULL," \
		"PASSWORD TEXT NOT NULL);";
	int rc = sqlite3_exec(db, sql_create_table, callback, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return 0;
	}
	else {
		fprintf(stdout, "Member table created successfully\n");
		return 1;
	}
}

// ȸ�� ���� �߰� �Լ�
int insert_member(sqlite3* db, const char* name, int age, const char* email, const char* user_id, const char* password) {
	char* zErrMsg = 0;
	char sql_insert_member[500];
	sprintf(sql_insert_member, "INSERT INTO MEMBER (NAME, AGE, EMAIL, USER_ID, PASSWORD) VALUES ('%s', %d, '%s', '%s', '%s');",
		name, age, email ? email : "", user_id, password);
	int rc = sqlite3_exec(db, sql_insert_member, callback, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return 0;
	}
	else {
		fprintf(stdout, "Member added successfully\n");
		return 1;
	}
}

// ���� �ʱ�ȭ �Լ�
int initialize_winsock() {
	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0) {
		fprintf(stderr, "WSAStartup failed with error code: %d\n", result);
		return -1;
	}
	return 0;
}

// ���� ���� �Լ�
int open_socket() {
	// Winsock �ʱ�ȭ
	if (initialize_winsock() != 0) {
		return -1;
	}

	int server_fd;
	struct sockaddr_in address;
	int addrlen = sizeof(address);

	// ���� ���� ����
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("socket failed");
		return -1;
	}
	printf("socket create success\n");

	// ���� �ɼ� ����
	int opt = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) < 0) {
		perror("setsockopt");
		return -1;
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	// ���Ͽ� �ּ� ���ε�
	if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
		int error_code = WSAGetLastError();
		fprintf(stderr, "bind failed with error code: %d\n", error_code);
		perror("bind failed");
		return -1;
	}

	// ������ ��� ����
	if (listen(server_fd, SOMAXCONN) < 0) {
		perror("listen");
		return -1;
	}
	printf("socket ready\n");

	return server_fd;
}

// Ŭ���̾�Ʈ ��û ó�� �Լ�
DWORD WINAPI handle_client(int client_socket) {
	char buffer[MAX_BUFFER_SIZE];
	int bytes_received;
	int run = 1;

	// Ŭ���̾�Ʈ�κ��� ��û�� ����
	do {
		printf("%d�� Ŭ���̾�Ʈ ��û���\n", client_socket);
		bytes_received = recv(client_socket, buffer, MAX_BUFFER_SIZE, 0);
		if (bytes_received == SOCKET_ERROR) {
			perror("recv failed");
			return;
		}
		buffer[bytes_received] = '\0'; // ���ڿ� ���� NULL �߰�
		printf("%d�� Ŭ���̾�Ʈ ��û : %s\n", client_socket, buffer);

		// Ŭ���̾�Ʈ�� ��� ����
		int bytes_sent = send(client_socket, buffer, bytes_received, 0);
		if (bytes_sent == SOCKET_ERROR) {
			perror("send failed");
			return;
		}
	} while (run);
}