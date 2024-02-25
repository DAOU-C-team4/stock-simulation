#include <stdio.h>
#include <stdlib.h>
#include "client_member.h"

// �α����� �ֽİ��� Ȩ
stock_home(SOCKET client_fd) {
	do {
		int select = 0;
		printf("\n(1.�ֽ� �ż� / 2.�ֽ� �ŵ� / 3.�α׾ƿ�)\n");
		printf("���ϴ� �۾��� �������ּ��� : ");
		scanf("%d%*c", &select);

		printf("\n========================================================================\n");
		switch (select)
		{
		case 1:
			buyStock(client_fd);
			WaitForSingleObject(event, INFINITE); // ��ȣ ���
			break;
		case 2:
			sellStock(client_fd);
			WaitForSingleObject(event, INFINITE); // ��ȣ ���
			break;
		case 3:
			printf("�Ÿ� ����\n");
			return;
		default:
			printf("\n1, 2, 3�� �� �ϳ��� �Է��ϼ���\n");
			continue;
		}
	} while (1 != 0);
}

//�ֽ� �ż� �Լ�
buyStock(SOCKET client_fd) {
	RequestData req_data;
	req_data.select = 6;

	printf("�ż��� ���� ��ȣ�� �Է��ϼ��� : ");
	scanf("%d%*c", &req_data.stock_data.stock_id);
	printf("�ż��� ������ �Է��ϼ��� : ");
	scanf("%d%*c", &req_data.stock_data.stock_count);

	// ������ ����
	int bytes_sent = send(client_fd, (RequestData*)&req_data, sizeof(req_data), 0);
	if (bytes_sent == SOCKET_ERROR) {
		fprintf(stderr, "Send failed\n");
		return 1;
	}
}

//�ֽ� �ŵ� �Լ�
sellStock(SOCKET client_fd) {
	RequestData req_data;
	req_data.select = 7;

	printf("�ŵ��� ���� ��ȣ�� �Է��ϼ��� : ");
	scanf("%d%*c", &req_data.stock_data.stock_id);
	printf("�ŵ��� ������ �Է��ϼ��� : ");
	scanf("%d%*c", &req_data.stock_data.stock_count);

	int bytes_sent = send(client_fd, (RequestData*)&req_data, sizeof(req_data), 0);
	if (bytes_sent == SOCKET_ERROR) {
		fprintf(stderr, "Send failed\n");
		return 1;
	}
}