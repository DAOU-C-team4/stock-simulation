#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"
#include <openssl/sha.h> // OpenSSL 라이브러리 헤더
#include <WinSock2.h>    // 네트워크 소켓 관련 헤더
#include <WS2tcpip.h>    // TCP/IP 통신을 위한 헤더
#pragma comment(lib, "Ws2_32.lib") // Windows 소켓 라이브러리 링크

#define PORT 8080
#define MAX_BUFFER_SIZE 1024
#define FD_SETSIZE 100
#define MAX_ID_LENGTH 50
#define MAX_PASSWORD_LENGTH 50
#define MAX_NAME_LENGTH 50
#define MAX_SESSION_LENGTH 200

// 함수 선언
static int callback(void* NotUsed, int argc, char** argv, char** azColName);
sqlite3* open_database(const char* db_name);
int create_member_table(sqlite3* db);
int insert_member(sqlite3* db, const char* name, int age, const char* email, const char* user_id, const char* password);
int open_socket();
DWORD WINAPI handle_client(LPVOID lpParam);

// 구조체 선언
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

// 전역 소켓 관리
SOCKET client_sockets[FD_SETSIZE];
int num_clients = 0;

int main(int argc, char* argv[]) {
	// 1. 데이터베이스 열기
	sqlite3* db = open_database("customer.db");
	if (!db) {
		return 1;
	}

	// 2. 서버 소켓열기
	int server_fd, new_socket, valread;
	server_fd = open_socket();
	if (server_fd == -1) {
		fprintf(stderr, "Failed to open socket\n");
		return 1;
	}

	// 클라이언트의 연결을 기다림
	while (1) {
		// 연결대기
		int new_socket = accept(server_fd, NULL, NULL);
		// 연결완료
		client_sockets[num_clients++] = new_socket;
		printf("\n");
		if (new_socket == -1) {
			perror("accept failed");
			free(new_socket);
			continue;
		}

		// 클라이언트가 연결되었음을 알림
		printf("%d번 클라이언트 연결\n", new_socket);

		// 클라이언트 요청을 처리하는 새로운 스레드 생성 (handle_client 함수에서 분기)
		DWORD dwThreadId;
		HANDLE hThread = CreateThread(NULL, 0, handle_client, new_socket, 0, &dwThreadId);
		if (hThread == NULL) {
			perror("Thread creation failed");
			free(new_socket);
			continue;
		}
	}

	// 서버 소켓 종료
	closesocket(new_socket);


	///////// 개발자용 //////////
	//// 고객정보 테이블 생성
	//create_member_table(db);
	//// 고객 정보 추가
	//insert_member(db, "John Doe", 30, "john@example.com", "johndoe", "password123");

	return 0;
}

// 콜백 함수 - SELECT 쿼리 결과를 처리하는 함수
static int callback(void* NotUsed, int argc, char** argv, char** azColName) {
	int i;
	for (i = 0; i < argc; i++) {
		printf("%s: %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}

// 1.1 데이터베이스 연결 함수
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

// 1.2 회원 테이블 생성 함수
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

// 1.3 회원 정보 추가 함수
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

// 1.4 회원 정보 삭제 함수

// 1.5 회원 로그인 함수

// 1.6 회원 로그아웃 함수

// 2.0 소켓 초기화 함수
int initialize_winsock() {
	WSADATA wsaData;
	int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0) {
		fprintf(stderr, "WSAStartup failed with error code: %d\n", result);
		return -1;
	}
	return 0;
}

// 2.1 소켓 열기 함수
int open_socket() {
	// Winsock 초기화
	if (initialize_winsock() != 0) {
		return -1;
	}

	int server_fd;
	struct sockaddr_in address;
	int addrlen = sizeof(address);

	// 서버 소켓 생성
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		perror("socket failed");
		return -1;
	}
	printf("socket create success\n");

	// 소켓 옵션 설정
	int opt = 1;
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) < 0) {
		perror("setsockopt");
		return -1;
	}

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	// 소켓에 주소 바인딩
	if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
		int error_code = WSAGetLastError();
		fprintf(stderr, "bind failed with error code: %d\n", error_code);
		perror("bind failed");
		return -1;
	}

	// 서버가 듣기 시작
	if (listen(server_fd, SOMAXCONN) < 0) {
		perror("listen");
		return -1;
	}
	printf("socket ready\n");

	return server_fd;
}

// 2.2 클라이언트 요청 처리 함수
DWORD WINAPI handle_client(int client_socket) {
	char buffer[MAX_BUFFER_SIZE];
	int bytes_received;
	int run = 1;

	// 클라이언트로부터 요청 대기
	do {
		printf("%d번 클라이언트 요청대기\n", client_socket);
		bytes_received = recv(client_socket, buffer, MAX_BUFFER_SIZE, 0);
		if (bytes_received == SOCKET_ERROR) {
			perror("recv failed");
			return;
		}

		// 클라이언트로부터 받은 데이터를 req_data 구조체로 형변환
		RequestData* req_data = (RequestData*)buffer;

		// 요청에 따라 다른 작업 실행
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
			// 주식 관련 데이터 받기
			break;
		}

		// 클라이언트로 결과 전송
		int bytes_sent = send(client_socket, buffer, bytes_received, 0);
		if (bytes_sent == SOCKET_ERROR) {
			perror("send failed");
			return;
		}
	} while (run);
}

// 2.2.1 클라이언트 요청 - 회원 가입
add_member(RequestData* req_data) {
	// 회원 db 등록
	printf("\n선택 : %d (회원가입)\n", req_data->select);
	printf("받은 아이디: %s", req_data->id);
	printf("받은 비밀번호: %s", req_data->password);
	printf("받은 이름: %s\n", req_data->name);
}

// 2.2.2 클라이언트 요청 - 회원 탈퇴
del_member(RequestData* req_data) {
	// 회원 db 삭제
	printf("\n선택 : %d (회원탈퇴)\n", req_data->select);
	printf("받은 아이디: %s", req_data->id);
	printf("받은 비밀번호: %s", req_data->password);
}

// 2.2.3 클라이언트 요청 - 로그인
login(RequestData* req_data) {
	// 로그인
	printf("\n선택 : %d (로그인)\n", req_data->select);
	printf("받은 아이디: %s", req_data->id);
	printf("받은 비밀번호: %s", req_data->password);
}

// 2.2.4 클라이언트 요청 - 로그아웃
logout(RequestData* req_data) {
	// 로그아웃
	printf("\n선택 : %d (로그아웃)\n", req_data->select);
}

// 2.2.5 클라이언트 요청 - 주식 매수

// 2.2.6 클라이언트 요청 - 주식 매도


// 2.3.1 클라이언트 요청 - 주식 정보 실시간

// 2.3.2 클라이언트 요청 - 주식 매매정보 실시간