#include <stdio.h>
#include <stdlib.h>
#include "client_member.h"

// �α����� �ֽİ��� Ȩ
stock_home(SOCKET client_fd) {
	do {
		int select = 0;
		printf("\n(1.�ֽ� �ż� / 2.�ֽ� �ŵ� / 3.�Ÿ� ����)\n");
		printf("���ϴ� �۾��� �������ּ��� : ");
		scanf("%d%*c", &select);

		printf("\n========================================================================\n");
		switch (select)
		{
		case 1:
			buyStock(client_fd);
			break;
		case 2:
			sellStock(client_fd);
			break;
		case 3:
			printf("�Ÿ� ����\n");
			return;
		default:
			printf("\n1, 2, 3�� �� �ϳ��� �Է��ϼ���\n");
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
		ResponseData* Res_data = (ResponseData*)received_message;
		
		printf("\n�����κ��� ����:\n");
		printf("========================================================================\n");
		printf("%s\n", Res_data->msg);
		//printf("   �����ȣ: %d\n", Res_data->select);
		//printf("   �����̸�: %s\n", Res_data->stock_name);
		//printf("   ���԰���: %d\n", Res_data->stock_price);
		//printf("   ��   ��: %d\n", Res_data->stock_count);
		printf("========================================================================\n");

	} while (1 != 0);
}

//�ֽ� �ż� ��û �Լ�
buyStock(SOCKET client_fd) {
	RequestData Req_data;
	Req_data.select = 5; //1. ȸ������, 2. ȸ��Ż��, 3. �α���, 4. �α׾ƿ�, 5. �Ÿ�
	Req_data.stock_data.select = 1; // �Ÿ� �б� => �ż�: 1, �ŵ� : 2 

	printf("�ż��� ���� ��ȣ�� �Է��ϼ��� : ");
	scanf("%d%*c", &(Req_data.stock_data.stock_id));
	printf("�ż��� ������ �Է��ϼ��� : ");
	scanf("%d%*c", &(Req_data.stock_data.stock_count));

	// ������ ����, 
	int bytes_sent = send(client_fd, (RequestData*)&Req_data, sizeof(Req_data), 0);
	if (bytes_sent == SOCKET_ERROR) {
		fprintf(stderr, "Send failed\n");
		return 1;
	}
}

//�ֽ� �ŵ� ��û �Լ�
sellStock(SOCKET client_fd) {
	RequestData Req_data;
	Req_data.select = 5; //1. ȸ������, 2. ȸ��Ż��, 3. �α���, 4. �α׾ƿ�, 5. �Ÿ�
	Req_data.stock_data.select = 2; // �Ÿ� �б� => �ż�: 1, �ŵ� : 2 

	printf("�ŵ��� ���� ��ȣ�� �Է��ϼ��� : ");
	scanf("%d%*c", &(Req_data.stock_data.stock_id));
	printf("�ŵ��� ������ �Է��ϼ��� : ");
	scanf("%d%*c", &(Req_data.stock_data.stock_count));

	// ������ ����, 
	int bytes_sent = send(client_fd, (RequestData*)&Req_data, sizeof(Req_data), 0);
	if (bytes_sent == SOCKET_ERROR) {
		fprintf(stderr, "Send failed\n");
		return 1;
	}
}