#include "server_db.h"
#include "server_socket.h"
#include "server_request.h"

sqlite3* db;

// �������� ����
SOCKET client_sockets[FD_SETSIZE];
int client_tf[FD_SETSIZE] = { 0 };
num_clients = 0;

int main(int argc, char* argv[]) {
	// 1. �����ͺ��̽� ����
	if(!db)
		db = open_database("customer.db");
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
	// 2.1 Ŭ���̾�Ʈ�� ������ ��ٸ�
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
	// 2.2 ���� ���� ����
	closesocket(new_socket);


	/****************** 1.1 �ʱ� ���� (���� �ּ�ó���� ����) ******************/
	//// ������ ���̺� ����
	//create_member_table(db);
	//create_stock_table(db);
	//create_account_table(db);
	//// �� ���� �߰�
	//db_insert_member(db, "name", "test", "1234");
	//// �ֽ� ���� �߰�
	//insert_stock_dummyData(db);

	return 0;
}
