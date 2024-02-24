#include "client_member.h"
#include "client_stock.h"
#include "client_socket.h"

// ������ ����
char access[31] = "NONE";

int main(int argc, char* argv[]) {
	// 0. ���Ͽ���
	SOCKET client_fd = connect_to_server();

	// 1. Ȩȭ�鿡�� ���ϴ� �۾�����
	select_task_home(client_fd);

	// end. ��������
	closesocket(client_fd);
	WSACleanup();

	return 0;
}

// 1. Ȩȭ�� �۾����� �Լ�
select_task_home(SOCKET client_fd) {
	// �⺻ ����
	int run = 1;
	int isLogin = 0; // 1�� ��� �α��� �Ϸ�
	char message[MAX_BUFFER_SIZE];
	printf("\n�ݰ����ϴ�. Ű���������Դϴ�.\n");

	// Ȩ �޴� �ݺ�
	do {
		// ���� ���� ��Ƽ������ ����

		// �α��ν� - �ֽĸŸ�
		if (strcmp(access, "NONE") && strcmp(access, "CLEAR")) {
			stock_home(client_fd); // �Ÿ����Ḧ ������ ��, �Ʒ��� �α׾ƿ� �Լ��� �����
			logout(client_fd, access);
			printf("���� ����� ������ �б�: %s\n", access);
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
			break;
		case 2:
			printf("ȸ��Ż��\n\n");
			del_member(client_fd);
			break;
		case 3:
			printf("�α���\n\n");
			login(client_fd);
			break;
		case 4:
			printf("\n���α׷��� �����մϴ�. ���� �Ϸ� �Ǽ��� :)\n");
			exit(1);
			break;
		default:
			printf("\n�߸��� ��ȣ�Դϴ�.\n");
			continue;
		}

		// �����κ��� ����
		char received_message[MAX_BUFFER_SIZE];
		int bytes_received = recv(client_fd, received_message, MAX_BUFFER_SIZE, 0);
		if (bytes_received == SOCKET_ERROR) {
			perror("recv failed");
			return 1;
		}
		// �����κ��� ���� �޽��� ���
		ResponseData* res_data = (ResponseData*)received_message;
		if (strcmp(res_data->session, "NONE") && strcmp(res_data->session, "CLEAR")) {
			strcpy(access, res_data->session);
		}
		printf("\n�����κ��� ����:\n");
		printf("   select: %d\n", res_data->select);
		printf("   session: %s\n", res_data->session);
		printf("   msg: %s\n", res_data->msg);
		printf("���� ����� ������ �б�: %s\n", access);

		printf("========================================================================\n");
	} while (run);
}
