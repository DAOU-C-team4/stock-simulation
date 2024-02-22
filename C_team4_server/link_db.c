#include "db_connect.h"
#include "socket_connect.h"
#include "member.h"

sqlite3* db;

SOCKET client_sockets[FD_SETSIZE];
int num_clients = 0;

int main(int argc, char* argv[]) {

	// 1. 데이터베이스 열기
	db = open_database("customer.db");
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
		int new_socket = accept(server_fd, NULL, NULL);
		client_sockets[num_clients++] = new_socket;
		printf("\n");
		if (new_socket == -1) {
			perror("accept failed");
			free(new_socket);
			continue;
		}

		// 클라이언트가 연결되었음을 알림
		printf("%d번 클라이언트 연결\n", new_socket);

		// 클라이언트 요청을 처리하는 새로운 스레드 생성
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
