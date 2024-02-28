#include "client_member.h"
#include "client_stock.h"

delay(clock_t delay_time)
{
	clock_t start = clock(); //clock �Լ��� 1/1000
	while (clock() - start < delay_time);
	return 0;

}

/**************** �ֽ� Ȩ ****************/
// 0. �α����� �ֽİ��� Ȩ
stock_home(SOCKET client_fd, char* access) {
	char send_access[MAX_SESSION_LENGTH];
	strcpy(send_access, access);
	do {
		int select;
		printf(">> �ֽ� ���� �̿� <<\n");
		printf("\n(1.�ֽ� �ż� / 2.�ֽ� �ŵ� / 3.�α׾ƿ�)\n");
		gotoxy(0, 18);
		printf("\n===================================================================================================================\n\n");
		select = getInputInteger(" ���ϴ� �۾��� �������ּ��� : ");

		system("cls");
		printf(">> �ֽ� ���� �̿� <<\n\n");

		if(select!=3)
			req_allStock(client_fd, send_access);

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
			printf("\n1, 2, 3�� �� �ϳ��� �Է��ϼ���\n\n");
			continue;
		};

	} while (1!=0);

	return 0;
}

/**************** �ֽ� ���� ��û �Լ� ****************/
// 1.0 �ֽ� �ǽð� ��ȸ ��û
req_allStock(SOCKET client_fd, char* access) {
	RequestData req_data;
	req_data.select = 200;
	strcpy(req_data.session, access);

	// ������ ����
	int bytes_sent = send(client_fd, (RequestData*)&req_data, sizeof(req_data), 0);
	if (bytes_sent == SOCKET_ERROR) {
		fprintf(stderr, "Send failed\n");
		return 1;
	}

	return 0;
}
// 1.1 �ֽ� �ż� ��û
req_buyStock(SOCKET client_fd, char* access) {
	printf("\n1. �ֽ� �ż�\n\n");
	RequestData req_data;
	req_data.select = 201;
	strcpy(req_data.session, access);

	gotoxy(0, 18);
	printf("\n===================================================================================================================\n\n");
	req_data.stock_data.stock_id = getInputInteger("�ż��� ���� ��ȣ�� �Է��ϼ��� : ");
	if (req_data.stock_data.stock_id > 10 || req_data.stock_data.stock_id < 0) {
		req_data.stock_data.stock_id = 100;
	}
	do {
		/*printf("�ż��� ���� ������ �Է��ϼ��� : ");
	scanf("%d%*c", &req_data.stock_data.stock_count);*/
		req_data.stock_data.stock_count = getInputInteger("�ż��� ������ �Է��ϼ��� : ");
	} while (req_data.stock_data.stock_count < 0);

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
	printf("\n2. �ֽ� �ŵ�\n\n");
	RequestData req_data;
	req_data.select = 202;
	strcpy(req_data.session, access);
	gotoxy(0, 18);
	printf("\n===================================================================================================================\n\n");

	//printf("�ŵ��� ���� ��ȣ�� �Է��ϼ��� : ");
	//scanf("%d%*c", &req_data.stock_data.stock_id);
	req_data.stock_data.stock_id = getInputInteger("�ŵ��� ���� ��ȣ�� �Է��ϼ��� : ");
	if (req_data.stock_data.stock_id > 10 || req_data.stock_data.stock_id < 0) {
		req_data.stock_data.stock_id = 100;
	}
	do {
		/*printf("�ŵ��� ������ �Է��ϼ��� : ");
		scanf("%d%*c", &req_data.stock_data.stock_count);*/
		//req_data.stock_data.stock_count = getInputInteger("�ŵ��� ������ �Է��ϼ���: ");
		req_data.stock_data.stock_count = getInputInteger("�ŵ��� ������ �Է��ϼ��� : ");
	} while (req_data.stock_data.stock_count < 0);

	int bytes_sent = send(client_fd, (RequestData*)&req_data, sizeof(req_data), 0);
	if (bytes_sent == SOCKET_ERROR) {
		fprintf(stderr, "Send failed\n");
		return 1;
	}

	WaitForSingleObject(event, INFINITE); // ��ȣ ���
	return 0;
}
//// 1.3 balance ��ȸ ��û
//req_getBalance(SOCKET client_fd, char* access) {
//	RequestData req_data;
//	req_data.select = 202;
//	strcpy(req_data.session, access);
//	int bytes_sent = send(client_fd, (RequestData*)&req_data, sizeof(req_data), 0);
//	if (bytes_sent == SOCKET_ERROR) {
//		fprintf(stderr, "Send failed\n");
//		return 1;
//	}
//
//	WaitForSingleObject(event, INFINITE); // ��ȣ ���
//	return 0;
//}

/**************** �ֽ� ���� ���� �Լ� ****************/
// 2.0 �ֽ� ���� ��ȸ
res_allStock(ResponseData* res_data) {
	//�α��� ���Ŀ��� stock_home���� ���� ������, ��� �۾� �Ŀ��� stock_home���� ���߿� �����Ƿ�
	//������ ���� Ŀ�� ��ġ�� ����ߴٰ� �����ؾ��Ѵ�.
	delay(100);
	CONSOLE_SCREEN_BUFFER_INFO presentCur; // �ܼ� ���â�� ������ ��� ���ؼ� ������ ����ü 	
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &presentCur);  //���� Ŀ���� ��ġ ������ �����ϴ� �Լ�	
	int pre_x = presentCur.dwCursorPosition.X + 1;
	int pre_y = presentCur.dwCursorPosition.Y + 1;

	int y = 1, c = 0;

	// �ֽ����� �ް� ����
	gotoxy(50, y);
	printf(">> �ǽð� �ֽ����� <<");
	y += 2;
	gotoxy(50, y);	
	printf("[ �ֽ����� ���� ]");
	gotoxy(50, ++y);
	printf("=================================================================");
	gotoxy(50, ++y);
	printf("| �����ȣ |        �����        |  ���簡��  |  ���Ű��ɼ���  |");
	gotoxy(50, ++y);
	printf("=================================================================");
	for (int i = 0; i < MAX_STOCK_RES_LENGTH; i++) {
		gotoxy(50, ++y);
		if (!res_data->stock_arr[i].stock_id) break;
		printf("|  %5d   | %20s | %10d |    %5d       |",
			res_data->stock_arr[i].stock_id, res_data->stock_arr[i].stock_company_name,
			res_data->stock_arr[i].stock_price, res_data->stock_arr[i].stock_count);
	}
	gotoxy(50, ++y);
	printf("=================================================================");

	//���� ��ǥ�� �ű��!!
	gotoxy(pre_x, pre_y);
	return 0;
}
// 2.1 �ֽ� �ż� ����
res_buyStock(ResponseData* res_data) {
	system("cls");
	printf("%s\n", res_data->msg);
	printf("\n=================================\n\n");
	// ���� �ֽ� �� �ܰ� ���

	return 0;
}
// 2.2 �ֽ� �ŵ� ����
res_sellStock(ResponseData* res_data) {
	system("cls");
	printf("%s\n", res_data->msg);
	printf("\n=================================\n\n");
	// ���� �ֽ� �� �ܰ� ���

	return 0;
}
//// 2.3 balance ��ȸ ����
//res_getBalance(ResponseData* res_data) {
//	printf("%d\n", res_data->);
//	return 0;
//}
