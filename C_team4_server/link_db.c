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
#define MAX_ID_LENGTH 50
#define MAX_PASSWORD_LENGTH 50
#define MAX_NAME_LENGTH 50
#define MAX_SESSION_LENGTH 200

// �Լ� ����
static int callback(void* NotUsed, int argc, char** argv, char** azColName);
sqlite3* open_database(const char* db_name);
int create_member_table(sqlite3* db);
int insert_member(sqlite3* db, const char* name, int age, const char* email, const char* user_id, const char* password);
int open_socket();
DWORD WINAPI handle_client(LPVOID lpParam);

// ����ü ����
typedef struct {
	int sample;
} STOCK_DATA;
typedef struct {
	int select;
	char name[MAX_NAME_LENGTH];
	char id[MAX_ID_LENGTH];
	char password[MAX_PASSWORD_LENGTH];
	char session[MAX_SESSION_LENGTH];
	STOCK_DATA stock_data;
} RequestData;

// ���� ���� ����
SOCKET client_sockets[FD_SETSIZE];
int num_clients = 0;

int main(int argc, char* argv[]) {
	// 1. �����ͺ��̽� ����
	sqlite3* db = open_database("customer.db");
	if (!db) {
		return 1;
	}

	// 2. ���� ���Ͽ���
	int server_fd, new_socket, valread;
	server_fd = open_socket();
	if (server_fd == -1) {
		fprintf(stderr, "Failed to open socket\n");
		return 1;
	}

	// Ŭ���̾�Ʈ�� ������ ��ٸ�
	while (1) {
		// ������
		int new_socket = accept(server_fd, NULL, NULL);
		// ����Ϸ�
		client_sockets[num_clients++] = new_socket;
		printf("\n");
		if (new_socket == -1) {
			perror("accept failed");
			free(new_socket);
			continue;
		}

		// Ŭ���̾�Ʈ�� ����Ǿ����� �˸�
		printf("%d�� Ŭ���̾�Ʈ ����\n", new_socket);

		// Ŭ���̾�Ʈ ��û�� ó���ϴ� ���ο� ������ ���� (handle_client �Լ����� �б�)
		DWORD dwThreadId;
		HANDLE hThread = CreateThread(NULL, 0, handle_client, new_socket, 0, &dwThreadId);
		if (hThread == NULL) {
			perror("Thread creation failed");
			free(new_socket);
			continue;
		}
	}

	// ���� ���� ����
	closesocket(new_socket);


	///////// �����ڿ� //////////
	//// ������ ���̺� ����
	//create_member_table(db);
	//// �� ���� �߰�
	//insert_member(db, "John Doe", 30, "john@example.com", "johndoe", "password123");

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

// 1.1 �����ͺ��̽� ���� �Լ�
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

// 1.2 ȸ�� ���̺� ���� �Լ�
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

// 1.3 ȸ�� ���� �߰� �Լ�
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

// 1.4 ȸ�� ���� ���� �Լ�

// 1.5 ȸ�� �α��� �Լ�

// 1.6 ȸ�� �α׾ƿ� �Լ�

// 2.0 ���� �ʱ�ȭ �Լ�
int initialize_winsock() {
	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0) {
		fprintf(stderr, "WSAStartup failed with error code: %d\n", result);
		return -1;
	}
	return 0;
}

// 2.1 ���� ���� �Լ�
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

// 2.2 Ŭ���̾�Ʈ ��û ó�� �Լ�
DWORD WINAPI handle_client(int client_socket) {
	char buffer[MAX_BUFFER_SIZE];
	int bytes_received;
	int run = 1;

	// Ŭ���̾�Ʈ�κ��� ��û ���
	do {
		printf("%d�� Ŭ���̾�Ʈ ��û���\n", client_socket);
		bytes_received = recv(client_socket, buffer, MAX_BUFFER_SIZE, 0);
		if (bytes_received == SOCKET_ERROR) {
			perror("recv failed");
			return;
		}

		// Ŭ���̾�Ʈ�κ��� ���� �����͸� req_data ����ü�� ����ȯ
		RequestData* req_data = (RequestData*)buffer;

		// ��û�� ���� �ٸ� �۾� ����
		switch (req_data->select)
		{
		case 1:
			add_member(req_data);
			break;
		case 2:
			del_member(req_data);
			break;
		case 3:
			login(req_data);
			break;
		case 4:
			logout(req_data);
			break;
		case 5:
			// �ֽ� ���� ������ �ޱ�
			break;
		}

		// Ŭ���̾�Ʈ�� ��� ����
		int bytes_sent = send(client_socket, buffer, bytes_received, 0);
		if (bytes_sent == SOCKET_ERROR) {
			perror("send failed");
			return;
		}
	} while (run);
}

// 2.2.1 Ŭ���̾�Ʈ ��û - ȸ�� ����
add_member(RequestData* req_data) {
	// ȸ�� db ���
	printf("\n���� : %d (ȸ������)\n", req_data->select);
	printf("���� ���̵�: %s", req_data->id);
	printf("���� ��й�ȣ: %s", req_data->password);
	printf("���� �̸�: %s\n", req_data->name);
}

// 2.2.2 Ŭ���̾�Ʈ ��û - ȸ�� Ż��
del_member(RequestData* req_data) {
	// ȸ�� db ����
	printf("\n���� : %d (ȸ��Ż��)\n", req_data->select);
	printf("���� ���̵�: %s", req_data->id);
	printf("���� ��й�ȣ: %s", req_data->password);
}

// 2.2.3 Ŭ���̾�Ʈ ��û - �α���
login(RequestData* req_data) {
	// �α���
	printf("\n���� : %d (�α���)\n", req_data->select);
	printf("���� ���̵�: %s", req_data->id);
	printf("���� ��й�ȣ: %s", req_data->password);
}

// 2.2.4 Ŭ���̾�Ʈ ��û - �α׾ƿ�
logout(RequestData* req_data) {
	// �α׾ƿ�
	printf("\n���� : %d (�α׾ƿ�)\n", req_data->select);
}

// 2.2.5 Ŭ���̾�Ʈ ��û - �ֽ� �ż�

// 2.2.6 Ŭ���̾�Ʈ ��û - �ֽ� �ŵ�


// 2.3.1 Ŭ���̾�Ʈ ��û - �ֽ� ���� �ǽð�

// 2.3.2 Ŭ���̾�Ʈ ��û - �ֽ� �Ÿ����� �ǽð�