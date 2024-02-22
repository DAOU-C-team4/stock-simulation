#include "db_connect.h"
#include "socket_connect.h"
#include "member.h"

sqlite3* db;

SOCKET client_sockets[FD_SETSIZE];
int num_clients = 0;

int main(int argc, char* argv[]) {

	// 1. �����ͺ��̽� ����
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

	// ���� ���� ����
	closesocket(new_socket);


	///////// �����ڿ� //////////
	//// ������ ���̺� ����
	//create_member_table(db);
	//// �� ���� �߰�
	//insert_member(db, "John Doe", 30, "john@example.com", "johndoe", "password123");

	return 0;
}
