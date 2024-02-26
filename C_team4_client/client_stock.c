#include <stdio.h>
#include <stdlib.h>
#include "client_member.h"
#include <time.h>

/**************** �ֽ� Ȩ ****************/
// 0. �α����� �ֽİ��� Ȩ
stock_home(SOCKET client_fd, char* access) {
	char send_access[MAX_SESSION_LENGTH];
	strcpy(send_access, access);
	do {
		int select = 0;
		clearConsoleArea(0, 0, 50, 50);
		req_allStock(client_fd, send_access);
		gotoxy(0, 0);
		printf(">> �ֽ� ���� �̿� <<\n");
		//printf("\n(0. �ֽ� ��ȸ / 1.�ֽ� �ż� / 2.�ֽ� �ŵ� / 3.�α׾ƿ�)\n");
		printf("\n(1.�ֽ� �ż� / 2.�ֽ� �ŵ� / 3.�α׾ƿ�)\n");
		printf("���ϴ� �۾��� �������ּ��� : ");
		scanf("%d%*c", &select);
		//printf("stock_home() �׼�����ū: %s, %s\n", send_access, access);

		printf("\n=================================\n\n");
		switch (select)
		{
		//�ӽ�
		//case 0:
		//	req_allStock(client_fd, send_access);
		//	break;
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
// 1.0 �ֽ� �ǽð� ��ȸ ��û
req_allStock(SOCKET client_fd, char* access) {
	//printf("�ֽ� ��ȸ\n\n");
	RequestData req_data;
	req_data.select = 200;
	strcpy(req_data.session, access);

	// ������ ����
	int bytes_sent = send(client_fd, (RequestData*)&req_data, sizeof(req_data), 0);
	if (bytes_sent == SOCKET_ERROR) {
		fprintf(stderr, "Send failed\n");
		return 1;
	}

	WaitForSingleObject(event, INFINITE); // ��ȣ ���
	return 0;
}
// 1.1 �ֽ� �ż� ��û
req_buyStock(SOCKET client_fd, char* access) {
	/*clearConsoleArea(0, 0, 50, 50);
	printf(">> �ֽ� ���� �̿� <<\n");
	printf("1. �ֽ� �ż�\n\n");*/
	RequestData req_data;
	req_data.select = 201;
	strcpy(req_data.session, access);

	printf("�ż��� ���� ��ȣ�� �Է��ϼ��� : ");
	scanf("%d%*c", &req_data.stock_data.stock_id);
	do {
		printf("�ż��� ������ �Է��ϼ���: ");
		scanf("%d%*c", &req_data.stock_data.stock_count);
	} while (req_data.stock_data.stock_count < 0);

	printf("�Ѿ�� ���� : %s\n", req_data.session);

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
	/*clearConsoleArea(0, 0, 50, 50);
	printf(">> �ֽ� ���� �̿� <<\n");
	printf("2. �ֽ� �ŵ�\n\n");*/
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

	int x = 50, y = 1, c = 0;
	clearConsoleArea(50, 0, 120, 25);// (50, 0) ��ġ�κ��� 120*25 ũ���� ���� Ŭ����

	// �ֽ����� �ް� ����
	int stock_arr_size = sizeof(res_data->stock_arr) / sizeof(res_data->stock_arr[0]);
	gotoxy(50, y);
	printf(">> �ǽð� �ֽ����� <<");
	y += 2;
	gotoxy(50, y);
	if (!stock_arr_size) {
		printf("�ֽ������� �������� �ʽ��ϴ�.");
	}
	else {
		printf("[ �ֽ����� ���� ]");
		gotoxy(50, ++y);
		printf("=================================================================");
		gotoxy(50, ++y);
		printf("| �����ȣ |        �����        |  ���簡��  |  ���Ű��ɼ���  |");
		gotoxy(50, ++y);
		printf("=================================================================");
		for (int i = 0; i < stock_arr_size; i++) {
			gotoxy(50, ++y);
			printf("|  %5d   | %20s | %10d |    %5d       |",
				res_data->stock_arr[i].stock_id, res_data->stock_arr[i].stock_company_name,
				res_data->stock_arr[i].stock_price, res_data->stock_arr[i].stock_count);
		}
	}
	//���� ��ǥ�� �ű��!!
	return 0;
}
// 2.1 �ֽ� �ż� ����
res_buyStock(ResponseData* res_data) {
	printf("%s\n", res_data->msg);
	// ���� �ֽ� �� �ܰ� ���

	//res_allStock(res_data);
	return 0;
}
// 2.2 �ֽ� �ŵ� ����
res_sellStock(ResponseData* res_data) {
	printf("%s\n", res_data->msg);
	// ���� �ֽ� �� �ܰ� ���

	//res_allStock(res_data);
	return 0;
}