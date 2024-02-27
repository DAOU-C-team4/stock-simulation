#include "server_db.h"
#include "server_socket.h"
#include "server_request.h"

sqlite3* db;

// 전역소켓 관리
SOCKET client_sockets[FD_SETSIZE];
int client_tf[FD_SETSIZE] = { 0 };
num_clients = 0;

int main(int argc, char* argv[]) {
	// 1. 데이터베이스 열기
	if(!db)
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
	// 2.1 클라이언트의 연결을 기다림
	while (1) {
		SOCKET new_socket = accept(server_fd, NULL, NULL);
		for (int i = 0; i < FD_SETSIZE; i++) {
			if (client_tf[i] == 0) {
				client_sockets[i] = new_socket;
				client_tf[i] = 1;
				break;
			}
		}
		printf("\n");
		if (new_socket == -1) {
			perror("accept failed");
			free(new_socket);
			continue;
		}
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
	// 2.2 서버 소켓 종료
	closesocket(new_socket);


	/****************** 1.1 초기 세팅 (소켓 주석처리후 실행) ******************/
	//// 고객정보 테이블 생성
	//create_member_table(db);
	//create_stock_table(db);
	//create_account_table(db);
	//// 고객 정보 추가
	//db_insert_member(db, "name", "test", "1234");
	//// 주식 정보 추가
	//insert_stock_dummyData(db);

	return 0;
}
