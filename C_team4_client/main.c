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
	char access_tmp[50];

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

		printf("\n�ֳ� listen_thread �����κ��� ���� select (%d): ", res_data->select);
		if (res_data->select == 0) { //�ӽù���(res �ι� ���� ��???)
			//+�̰� �ذ��ϰ� �ֽĵ����� ��� �ٽ� ���ľ���
			//��� �������� ��±��� �ݿ��غ���
			continue;
		}
		if (res_data->select==200) {
			res_allStock(res_data);
		}
		if (strcmp(res_data->session, "NONE") && strcmp(res_data->session, "CLEAR" )) {
			strcpy(access, res_data->session);
			printf("\nlisten_thread���� session (%s): ", access);
		}
		printf("\n�����κ��� ���� (%d): ", res_data->select);
		printf("\nlisten_thread session (%s): ", access);

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
			res_login(res_data);
			break;
		case 4: // �α׾ƿ�
			res_logout(res_data, access);
			break;
		case 5: // ȸ�� ���� ��ȸ
			res_memberInfo(res_data, access);
			continue;
		case 200: // �ְ� �ǽð� ��ȸ
			res_allStock(res_data);
			continue;
		case 201: // �ֽ� �ż�
			res_buyStock(res_data);
			break;

		case 202: // �ֽ� �ŵ�
			res_sellStock(res_data);
			break;

		default:
			printf("\n�� �� ���� ��û�Դϴ�.\n");
			continue;
		}

		// �̺�Ʈ ��ȣ �߻�
		SetEvent(event);
	}
	return;
}

// 1.2 ��û ������
select_task_home(SOCKET client_fd) {
	// �⺻ ����
	int run = 1;
	char message[MAX_BUFFER_SIZE];
	printf("\n�ݰ����ϴ�. Ű���������Դϴ�.\n");

	// Ȩ �޴� �ݺ�
	do {
		// �α��ν� - �ֽĸŸ�
		if (strcmp(access, "NONE") && strcmp(access, "CLEAR")) {
			printf("select_task_home access : %s\n", access);
			stock_home(client_fd, access);
			// �ֽĸŸ� Ȩ���� ������ �α׾ƿ�
			req_logout(client_fd, access);
			printf("\n========================================================================\n");
			continue;
		}
		// �α��� �ƴҽ� - ȸ������
		int select = 0;
		printf("\n(1.ȸ������ / 2.ȸ��Ż�� / 3.�α��� / 4.����)\n");
		printf("���ϴ� �۾��� �������ּ��� : ");
		scanf("%d%*c", &select);
		printf("\n========================================================================\n\n");
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
		printf("\n========================================================================\n");
	} while (run);
}
