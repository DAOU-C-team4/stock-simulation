#include <stdio.h>
#include <stdlib.h>
#include "client_member.h"

/**************** �ֽ� Ȩ ****************/
// 0. �α����� �ֽİ��� Ȩ
stock_home(SOCKET client_fd, char* access) {
	char send_access[MAX_SESSION_LENGTH];
	strcpy(send_access, access);
	do {
		int select = 0;
		printf("\n(1.�ֽ� �ż� / 2.�ֽ� �ŵ� / 3.�α׾ƿ�)\n");
		printf("���ϴ� �۾��� �������ּ��� : ");
		scanf("%d%*c", &select);

		printf("\n========================================================================\n\n");
		switch (select)
		{
		case 1:
			req_buyStock(client_fd, send_access);
			break;
		case 2:
			req_sellStock(client_fd, send_access);
			break;
		case 3:
			printf("�Ÿ� ����. �ȳ��������� :)\n");
			return;
		default:
			printf("\n1, 2, 3�� �� �ϳ��� �Է��ϼ���\n");
			continue;
		}
	} while (1 != 0);

	return 0;
}

/**************** �ֽ� ���� ��û �Լ� ****************/
// 1.1 �ֽ� �ż� ��û
req_buyStock(SOCKET client_fd, char* access) {
	printf("�ֽ� �ż�\n\n");
	RequestData req_data;
	req_data.select = 201;
	strcpy(req_data.session, access);

	printf("�ż��� ���� ��ȣ�� �Է��ϼ��� : ");
	scanf("%d%*c", &req_data.stock_data.stock_id);
	do {
		printf("�ż��� ������ �Է��ϼ���: ");
		scanf("%d%*c", &req_data.stock_data.stock_count);
	} while (req_data.stock_data.stock_count < 0);

	// ������ ����
	int bytes_sent = send(client_fd, (RequestData*)&req_data, sizeof(req_data), 0);
	if (bytes_sent == SOCKET_ERROR) {
		fprintf(stderr, "Send failed\n");
		return 1;
	}

	WaitForSingleObject(event, INFINITE); // ��ȣ ���
	return 0;
}

// 1.2 �ֽ� �ŵ� ��û
req_sellStock(SOCKET client_fd, char* access) {
	printf("�ֽ� �ŵ�\n\n");
	RequestData req_data;
	req_data.select = 202;
	strcpy(req_data.session, access);


	printf("�ŵ��� ���� ��ȣ�� �Է��ϼ��� : ");
	scanf("%d%*c", &req_data.stock_data.stock_id);
	do {
		printf("�ŵ��� ������ �Է��ϼ���: ");
		scanf("%d%*c", &req_data.stock_data.stock_count);
	} while (req_data.stock_data.stock_count < 0);

	int bytes_sent = send(client_fd, (RequestData*)&req_data, sizeof(req_data), 0);
	if (bytes_sent == SOCKET_ERROR) {
		fprintf(stderr, "Send failed\n");
		return 1;
	}

	WaitForSingleObject(event, INFINITE); // ��ȣ ���
	return 0;
}

/**************** �ֽ� ���� ���� �Լ� ****************/
// 2.0 �ֽ� ���� ��ȸ
res_allStock(ResponseData* res_data) {
	// �ֽ����� �ް� ����
	printf("�ֽ����� ����\n");

	return 0;
}
// 2.1 �ֽ� �ż� ����
res_buyStock(ResponseData* res_data) {
	printf("�ֽĸż�\n");
	printf("%s\n", res_data->msg);
	// ���� �ֽ� �� �ܰ� ���

	return 0;
}
// 2.2 �ֽ� �ŵ� ����
res_sellStock(ResponseData* res_data) {
	printf("�ֽĸŵ�\n");
	printf("%s\n", res_data->msg);
	// ���� �ֽ� �� �ܰ� ���

	return 0;
}