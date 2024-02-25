#include "client_member.h"
#include "client_stock.h"
#include "client_socket.h"

// ������ ����
char access[31] = "NONE";

int main(int argc, char* argv[]) {
	// 0 ���Ͽ���
	SOCKET client_fd = connect_to_server();

	// 1.0 �̺�Ʈ ��ü ����
	event = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (event == NULL) {
		perror("Event creation failed");
		return 1;
	}

	// 1.1 ���� ������
	DWORD dwThreadId;
	HANDLE hThread = CreateThread(NULL, 0, listen_thread, client_fd, 0, &dwThreadId);
	if (hThread == NULL) {
		perror("Thread creation failed");
		return 1;
	}
	// 1.2 ��û ������
	select_task_home(client_fd);

	// end. ��������
	closesocket(client_fd);
	WSACleanup();

	return 0;
}

// 1.1 ���� ������
DWORD WINAPI listen_thread(SOCKET client_fd) {
	char received_message[MAX_BUFFER_SIZE];
	while (1) {
		int bytes_received = recv(client_fd, received_message, MAX_BUFFER_SIZE, 0);
		if (bytes_received == SOCKET_ERROR) {
			perror("recv failed");
			// ���� ó�� ���� �߰�
			break;
		}

		// �����κ��� ���� �޽��� ó��
		ResponseData* res_data = (ResponseData*)received_message;
		if (strcmp(res_data->session, "NONE") && strcmp(res_data->session, "CLEAR")) {
			strcpy(access, res_data->session);
		}
		printf("\n�����κ��� ����:\n");
		printf("   select: %d\n", res_data->select);
		printf("   session: %s\n", res_data->session);
		printf("   msg: %s\n", res_data->msg);
		printf("���� ����� ������ �б�: %s\n", access);

		// �̺�Ʈ ��ȣ �߻�
		SetEvent(event);
	}
	return;
}

// 1.2 ��û ������
select_task_home(SOCKET client_fd) {
	// �⺻ ����
	int run = 1;
	int isLogin = 0; // 1�� ��� �α��� �Ϸ�
	char message[MAX_BUFFER_SIZE];
	printf("\n�ݰ����ϴ�. Ű���������Դϴ�.\n");

	// Ȩ �޴� �ݺ�
	do {
		// �α��ν� - �ֽĸŸ�
		if (strcmp(access, "NONE") && strcmp(access, "CLEAR")) {
			stock_home(client_fd);
			logout(client_fd, access);
			WaitForSingleObject(event, INFINITE); // ��ȣ ���
			printf("========================================================================\n");
			continue;
		}
		// �α��� �ƴҽ� - ȸ������
		int select = 0;
		printf("\n(1.ȸ������ / 2.ȸ��Ż�� / 3.�α��� / 4.����)\n");
		printf("���ϴ� �۾��� �������ּ��� : ");
		scanf("%d%*c", &select);

		printf("\n========================================================================\n");
		switch (select)
		{
		case 1:
			printf("ȸ������\n\n");
			add_member(client_fd);
			WaitForSingleObject(event, INFINITE); // ��ȣ ���
			break;
		case 2:
			printf("ȸ��Ż��\n\n");
			del_member(client_fd);
			WaitForSingleObject(event, INFINITE); // ��ȣ ���
			break;
		case 3:
			printf("�α���\n\n");
			login(client_fd);
			WaitForSingleObject(event, INFINITE); // ��ȣ ���
			break;
		case 4:
			printf("\n���α׷��� �����մϴ�. ���� �Ϸ� �Ǽ��� :)\n");
			exit(1);
			break;
		default:
			printf("\n�߸��� ��ȣ�Դϴ�.\n");
			continue;
		}
		printf("========================================================================\n");
	} while (run);
}
