#include "server_request.h"
#include "server_socket.h"
#include "server_db.h"

sqlite3* db;

/**************** Ŭ���̾�Ʈ ��û �б� ****************/
// 2.2 Ŭ���̾�Ʈ ��û ó�� �Լ�
DWORD WINAPI handle_client(int client_socket) {
	char buffer[MAX_BUFFER_SIZE];
	int bytes_received;
	int run = 1;

	// Ŭ���̾�Ʈ�κ��� ��û ���
	do {
		printf("%d�� Ŭ���̾�Ʈ ��û���\n", client_socket);
		bytes_received = recv(client_socket, buffer, MAX_BUFFER_SIZE, 0);
		if (bytes_received == SOCKET_ERROR) {
			perror("recv failed");
			return;
		}

		// Ŭ���̾�Ʈ�κ��� ���� �����͸� req_data ����ü�� ����ȯ
		RequestData* req_data = (RequestData*)buffer;
		ResponseData res_data;
		ResponseData* res_data_ptr = &res_data;

		// ��û�� ���� �ٸ� �۾� ����
		switch (req_data->select)
		{
		case 1:
			add_member(req_data, res_data_ptr);
			break;
		case 2:
			del_member(req_data, res_data_ptr);
			break;
		case 3:
			login(req_data, res_data_ptr);
			break;
		case 4:
			logout(req_data, res_data_ptr);
		case 5:
			stock_trade(req_data, res_data_ptr);
			break;
		}

		// Ŭ���̾�Ʈ�� ��� ����
		int bytes_sent = send(client_socket, res_data_ptr, sizeof(res_data), 0);
		if (bytes_sent == SOCKET_ERROR) {
			perror("send failed");
			return;
		}
	} while (run);
	return;
}

/**************** ȸ�� ���� �Լ� ****************/
// 1.1 Ŭ���̾�Ʈ ��û - ȸ�� ����
int add_member(RequestData* req_data, ResponseData* res_data_ptr) {
	// ȸ�� db ���
	printf("\n���� : %d (ȸ������)\n", req_data->select);
	printf("���� ���̵�: %s", req_data->id);
	printf("���� ��й�ȣ: %s", req_data->password);
	printf("���� �̸�: %s", req_data->name);
	insert_member(db, req_data->name, req_data->id, req_data->password);
	// ���䵥���� ���
	res_data_ptr->select = 1;
	strcpy(res_data_ptr->session, "NONE");
	strcpy(res_data_ptr->msg, "ȸ������ �Ϸ�");
	return 0;
}

// 1.2 Ŭ���̾�Ʈ ��û - ȸ�� Ż��
int del_member(RequestData* req_data, ResponseData* res_data_ptr) {
	// ȸ�� db ����
	printf("\n���� : %d (ȸ��Ż��)\n", req_data->select);
	printf("���� ���̵�: %s", req_data->id);
	printf("���� ��й�ȣ: %s", req_data->password);
	delete_member(db, req_data->id, req_data->password);
	// ���䵥���� ���
	res_data_ptr->select = 2;
	strcpy(res_data_ptr->session, "NONE");
	strcpy(res_data_ptr->msg, "ȸ��Ż�� �Ϸ�");
	return 0;
}

// 1.3 Ŭ���̾�Ʈ ��û - �α���
int login(RequestData* req_data, ResponseData* res_data_ptr) {
	// �α���
	printf("\n���� : %d (�α���)\n", req_data->select);
	printf("���� ���̵�: %s", req_data->id);
	printf("���� ��й�ȣ: %s", req_data->password);
	char* access_key;
	access_key = member_login(db, req_data->id, req_data->password);
	// ���䵥���� ���
	res_data_ptr->select = 3;
	strcpy(res_data_ptr->session, access_key);
	strcpy(res_data_ptr->msg, "�α��� �Ϸ�");
	return 0;
}

// 1.4 Ŭ���̾�Ʈ ��û - �α׾ƿ�
int logout(RequestData* req_data, ResponseData* res_data_ptr) {
	// �α׾ƿ�
	printf("\n���� : %d (�α׾ƿ�)\n", req_data->select);
	printf("���� ����: %s", req_data->session);
	member_logout(db, req_data->session);
	// ���䵥���� ���
	res_data_ptr->select = 4;
	strcpy(res_data_ptr->session, "CLEAR");
	strcpy(res_data_ptr->msg, "�α׾ƿ� �Ϸ�");
	return 0;
}


/**************** �ֽİ��� �Լ� ****************/
// 2.1 Ŭ���̾�Ʈ ��û - �ֽ� �Ÿ�
int stock_trade(RequestData* req_data, ResponseData* res_data_ptr) {
	int trade_category = req_data->stock_data.select; // �ż� : 1, �ŵ� : 2, �Ÿ� ���� : 3

	if (trade_category == 1) {
		//�ֽ� �ż�
		stock_buy(req_data, res_data_ptr);
	} 
	else if (trade_category == 2) {
		//�ֽ� �ŵ�
		stock_sell(req_data, res_data_ptr);
	}
	else {
		//�Ÿ� ����
		strcpy(res_data_ptr->msg, "�ŸŸ� �����մϴ�");
	}

	return 0;
}

// 2.1.1 Ŭ���̾�Ʈ ��û - �ֽ� �ż�
stock_buy(RequestData* req_data, ResponseData* res_data_ptr) {
	
	printf("\n�Ÿż��� : %d (�ż�)\n", req_data->stock_data.select); //�ż�: 1, �ŵ� : 2
	printf("�ż� �����ȣ: %d\n", req_data->stock_data.stock_id);
	insert_buyOrder();

	// ���䵥���� ���
	res_data_ptr->select = 5;
	strcpy(res_data_ptr->msg, "�ż� ��û�� �������� ó����");
	
	return 0;
}

// 2.1.2 Ŭ���̾�Ʈ ��û - �ֽ� �ŵ�
stock_sell(RequestData* req_data, ResponseData* res_data_ptr) {

	printf("\n�Ÿż��� : %d (�ż�)\n", req_data->stock_data.select); //�ż�: 1, �ŵ� : 2
	printf("�ż� �����ȣ: %d\n", req_data->stock_data.stock_id);
	insert_sellOrder();

	// ���䵥���� ���
	res_data_ptr->select = 6; // ����: �ż� 5, �ŵ� 6
	strcpy(res_data_ptr->msg, "�ŵ� ��û�� �������� ó����");

	return 0;
}
// 2.2 Ŭ���̾�Ʈ ��û - �ֽ� ���� �ǽð�

// 2.3 Ŭ���̾�Ʈ ��û - �ֽ� �Ÿ����� �ǽð�
