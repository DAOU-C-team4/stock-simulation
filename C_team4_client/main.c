#include <stdio.h>
#include <stdlib.h>
#include "client_socket.h"
#include "client_member.h"

#define MAX_BUFFER_SIZE 1024

void clear_input_buffer();

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
		int select = 0;
		printf("\n(1.ȸ������ / 2.ȸ��Ż�� / 3.�α��� / 4.�α׾ƿ� / 5.����)\n");
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
			//�α��� �Լ� ����� isLogin�� ���� (0: ����, 1: ����)
			isLogin = login(client_fd);
			//�α��� ���� ��, �ֽ� �Ÿ� �Լ� ����
			if (isLogin) {
				stock_home(client_fd);
			}
			break;
		case 4:
			printf("�α׾ƿ�\n\n");
			logout(client_fd);
			isLogin = 0;
			break;
		case 5:
			printf("\n���α׷��� �����մϴ�. ���� �Ϸ� �Ǽ��� :)\n");
			exit(1);
			break;
		}

		// �����κ��� ����
		char received_message[MAX_BUFFER_SIZE];
		int bytes_received = recv(client_fd, received_message, MAX_BUFFER_SIZE, 0);
		if (bytes_received == SOCKET_ERROR) {
			perror("recv failed");
			return 1;
		}
		received_message[bytes_received] = '\0'; // ���ڿ� ���� NULL �߰�
		// �����κ��� ���� �޽��� ���
		printf("%s\n", received_message);

		printf("========================================================================\n");
	} while (run);
}

// �Է� ���� ���� �Լ�
void clear_input_buffer() {
	int c;
	while ((c = getchar()) != '\n' && c != EOF);
}