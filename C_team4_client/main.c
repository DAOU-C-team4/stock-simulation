#include "client_member.h"
#include "client_stock.h"
#include "client_socket.h"

// ������ ����
char access[31] = "NONE";

int main(int argc, char* argv[]) {
	// 0.1 ���Ͽ���
	SOCKET client_fd = connect_to_server();
	// 0.2 �̺�Ʈ ��ü ����
	event = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (event == NULL) {
		perror("Event creation failed");
		return 1;
	}

	// 1. ���� ������
	DWORD dwThreadId;
	HANDLE hThread = CreateThread(NULL, 0, listen_thread, client_fd, 0, &dwThreadId);
	if (hThread == NULL) {
		perror("Thread creation failed");
		return 1;
	}

	// 2. ��û ������
	select_task_home(client_fd);

	// end. ��������
	closesocket(client_fd);
	WSACleanup();
	return 0;
}

// 1.1 ���� ������
DWORD WINAPI listen_thread(SOCKET client_fd) {
	char received_message[MAX_BUFFER_SIZE];
	int select;

	while (1) {
		// ���� ���۸޼��� ����
		int bytes_received = recv(client_fd, received_message, MAX_BUFFER_SIZE, 0);
		if (bytes_received == SOCKET_ERROR) {
			perror("recv failed");
			// ���� ó�� ���� �߰�
			break;
		}
		// �����κ��� ���� �޽��� ó��
		ResponseData* res_data = (ResponseData*)received_message;

		if (strcmp(res_data->session, "NONE") && strcmp(res_data->session, "CLEAR")
			&& strcmp(res_data->session, "") && strcmp(res_data->session, "\0")
			&& res_data->session == NULL) {
			strcpy(access, res_data->session);
			printf("\nlisten_thread���� session (%s): ", access);
		}
		//printf("\n�����κ��� ���� (%d): \n", res_data->select);
		//printf("\nlisten_thread session (%s): ", access);

		// ��û�� �б�ó��
		select = res_data->select;
		switch (select) {
		case 1: // ȸ������
			res_add_member(res_data);
			break;
		case 2: // ȸ��Ż��
			res_del_member(res_data);
			break;
		case 3: // �α���
			res_login(res_data, access);
			break;
		case 4: // �α׾ƿ�
			res_logout(res_data, access);
			break;
		case 5: // ȸ�� ���� ��ȸ
			res_memberInfo(res_data, access);
			continue;
		case 200: // �ְ� �ǽð� ��ȸ
			res_allStock(res_data);
			break;
		case 201: // �ֽ� �ż�
			res_buyStock(res_data);
			break;
		case 202: // �ֽ� �ŵ�
			res_sellStock(res_data);
			break;

		default:
			//printf("\n�� �� ���� ��û�Դϴ�.\n");
			continue;
		}

		// �̺�Ʈ ��ȣ �߻�
		if (select != 200 && select != 0)
			SetEvent(event);
	}
	return;
}

// 1.2 ��û ������
select_task_home(SOCKET client_fd) {
	// �⺻ ����
	int run = 1;
	char message[MAX_BUFFER_SIZE];
	/*system("cls");
	printf("\n�ݰ����ϴ�. Ű���������Դϴ�.\n");*/

	// Ȩ �޴� �ݺ�
	do {
		// �α��ν� - �ֽĸŸ�
		if (strcmp(access, "NONE") && strcmp(access, "CLEAR")) {
			//printf("%s\n", res_data->msg);
			//printf("   session: %s\n", access);
			req_allStock(client_fd, access);
			stock_home(client_fd, access);
			// �ֽĸŸ� Ȩ���� ������ �α׾ƿ�
			req_logout(client_fd, access);
			printf("\n========================================================================\n");
			continue;
		}
		// �α��� �ƴҽ� - ȸ������
		int select = 0;
		//system("cls");
		printf("\n�ݰ����ϴ�. Ű���������Դϴ�.\n");
		printf("\n(1.ȸ������ / 2.ȸ��Ż�� / 3.�α��� / 4.����)\n");
		select = getInputInteger("���ϴ� �۾��� �������ּ��� : ");
		printf("\n====================================================\n\n");
		switch (select)
		{
		case 1: // ȸ������
			req_add_member(client_fd);
			break;
		case 2: // ȸ��Ż��
			req_del_member(client_fd);
			break;
		case 3: // �α���
			req_login(client_fd);
			break;
		case 4: // �α׾ƿ�
			printf("\n���α׷��� �����մϴ�. ���� �Ϸ� �Ǽ��� :)\n");
			run = 0;
			break;
		default:
			printf("\n�߸��� ��ȣ�Դϴ�.\n");
			continue;
		}
		printf("\n====================================\n");
	} while (run);
}
