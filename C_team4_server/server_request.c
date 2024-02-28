#include "server_request.h"
#include "server_socket.h"
#include "server_db.h"

static int init_stock(ResponseData* res_data_ptr) {
	STOCK_RES* result = db_allStock(db);
	if (result) {
		int stock_arr_size = sizeof(res_data_ptr->stock_arr) / sizeof(res_data_ptr->stock_arr[0]);

		for (int i = 0; i < stock_arr_size; i++) {
			/*printf("stock_id[%d] = %d\n", i, result[i].stock_id);
			printf("stock_name[%d] = %d\n", i, result[i].stock_name);
			printf("stock_company_name[%d] = %s\n", i, result[i].stock_company_name);
			printf("stock_price[%d] = %d\n", i, result[i].stock_price);
			printf("stock_count[%d] = %d\n\n", i, result[i].stock_count);*/

			res_data_ptr->stock_arr[i].stock_id = result[i].stock_id;
			res_data_ptr->stock_arr[i].stock_name = result[i].stock_name;
			strcpy(res_data_ptr->stock_arr[i].stock_company_name, result[i].stock_company_name);
			res_data_ptr->stock_arr[i].stock_price = result[i].stock_price;
			res_data_ptr->stock_arr[i].stock_count = result[i].stock_count;
		}
		return 0;
	}
}

/**************** Ŭ���̾�Ʈ ��û �б� ****************/
// 2.2 Ŭ���̾�Ʈ ��û ó�� �Լ�
DWORD WINAPI handle_client(SOCKET client_socket) {
	char buffer[MAX_BUFFER_SIZE];
	int bytes_received, bytes_sent;
	int run = 1;

	// Ŭ���̾�Ʈ�κ��� ��û ���
	do {
		printf("%d�� Ŭ���̾�Ʈ ��û���\n", client_socket);
		bytes_received = recv(client_socket, buffer, MAX_BUFFER_SIZE, 0);
		if (bytes_received == SOCKET_ERROR) {
			perror("recv failed");
			for (int i = 0; i < FD_SETSIZE; i++) {
				if (client_sockets[i] == client_socket) {
					client_tf[i] = 0;
				}
			}
			return 1;
		}

		// Ŭ���̾�Ʈ�κ��� ���� �����͸� req_data ����ü�� ����ȯ
		RequestData* req_data = (RequestData*)buffer;
		ResponseData res_data = { 0 };
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
			break;
		case 5:
			// ȸ�� ���� ��ȸ ��û
			break;
		case 200:
			allStock(req_data, res_data_ptr);
			break;
		case 201:
			buyStock(req_data, res_data_ptr);
			bytes_sent = send(client_socket, res_data_ptr, sizeof(res_data), 0);
			if (bytes_sent == SOCKET_ERROR) {
				perror("send buyStock() failed");
				return 1;
			}
			sendAllClnt(res_data_ptr);
			break;
		case 202:
			sellStock(req_data, res_data_ptr);
			bytes_sent = send(client_socket, res_data_ptr, sizeof(res_data), 0);
			if (bytes_sent == SOCKET_ERROR) {
				perror("send sellStock() failed");
				return 1;
			}
			sendAllClnt(res_data_ptr);
			break;
		default:
			// �߸��� ��û ó��
			break;
		}

		// Ŭ���̾�Ʈ�� ��� ����
		bytes_sent = send(client_socket, res_data_ptr, sizeof(res_data), 0);
		if (bytes_sent == SOCKET_ERROR) {
			perror("send failed");
			return 1;
		}
	} while (run);
	return 0;
}

/**************** ȸ�� ���� �Լ� ****************/
// 1.1 Ŭ���̾�Ʈ ��û - ȸ�� ����
int add_member(RequestData* req_data, ResponseData* res_data_ptr) {
	// ȸ�� db ���
	printf("\n���� : %d (ȸ������)\n", req_data->select);
	printf("���� ���̵�: %s\n", req_data->id);
	printf("���� ��й�ȣ: %s\n", req_data->password);
	printf("���� �̸�: %s\n", req_data->name);
	// ���䵥���� ���
	int result;
	result = db_insert_member(db, req_data->name, req_data->id, req_data->password);
	res_data_ptr->select = 1;
	strcpy(res_data_ptr->session, "NONE");
	if (result) {
		strcpy(res_data_ptr->msg, "ȸ������ �Ϸ�");
	}
	else {
		strcpy(res_data_ptr->msg, "ȸ������ ����");
	}
	return 0;
}

// 1.2 Ŭ���̾�Ʈ ��û - ȸ�� Ż��
int del_member(RequestData* req_data, ResponseData* res_data_ptr) {
	// ȸ�� db ����
	printf("\n���� : %d (ȸ��Ż��)\n", req_data->select);
	printf("���� ���̵�: %s\n", req_data->id);
	printf("���� ��й�ȣ: %s\n", req_data->password);
	// ���䵥���� ���
	int result;
	result = db_delete_member(db, req_data->id, req_data->password);
	res_data_ptr->select = 2;
	strcpy(res_data_ptr->session, "NONE");
	if (result) {
		strcpy(res_data_ptr->msg, "ȸ��Ż�� �Ϸ�");
	}
	else {
		strcpy(res_data_ptr->msg, "ȸ��Ż�� ����. ���̵� ��й�ȣ�� Ȯ�����ּ���.");
	}
	return 0;
}

// 1.3 Ŭ���̾�Ʈ ��û - �α���
int login(RequestData* req_data, ResponseData* res_data_ptr) {
	// �α���
	printf("\n���� : %d (�α���)\n", req_data->select);
	printf("���� ���̵�: %s\n", req_data->id);
	printf("���� ��й�ȣ: %s\n", req_data->password);
	char* access_key;
	access_key = db_login(db, req_data->id, req_data->password);
	// ���䵥���� ���
	res_data_ptr->select = 3;
	strcpy(res_data_ptr->session, access_key);
	if (!strcmp(access_key, "NONE")) {
		strcpy(res_data_ptr->msg, "�α��� ����. ���̵� ��й�ȣ�� Ȯ�����ּ���.");
	}
	else if (access_key == NULL) {
		strcpy(res_data_ptr->msg, "���� ����");
	}
	else {
		strcpy(res_data_ptr->msg, "�α��� ����");
	}

	init_stock(res_data_ptr);
	return 0;
}

// 1.4 Ŭ���̾�Ʈ ��û - �α׾ƿ�
int logout(RequestData* req_data, ResponseData* res_data_ptr) {
	// �α׾ƿ�
	printf("\n���� : %d (�α׾ƿ�)\n", req_data->select);
	printf("���� ����: %s\n", req_data->session);
	db_logout(db, req_data->session);
	// ���䵥���� ���
	res_data_ptr->select = 4;
	strcpy(res_data_ptr->session, "CLEAR");
	strcpy(res_data_ptr->msg, "�α׾ƿ� �Ϸ�");
	return 0;
}


/**************** �ֽİ��� �Լ� ****************/

// **Ŭ���̾�Ʈ ��û - ��ü ȸ������ ������
int sendAllClnt(ResponseData* res_data_ptr) {
	res_data_ptr->select = 200;
	init_stock(res_data_ptr);
	int bytes_sent;
	// Ŭ���̾�Ʈ�� ��� ����
	for (int i = 0; i < FD_SETSIZE; i++) {
		if (client_tf[i] == 1)
			bytes_sent = send(client_sockets[i], res_data_ptr, sizeof(*res_data_ptr), 0);
		if (bytes_sent == SOCKET_ERROR) {
			perror("send failed : ");
			return 1;
		}
	}
	return 0;
}

// (�۾��ؾ���!!!!!!!!) 2.0 ���� ��ȿ ����
int checkSession(char* session) {
	printf("������ �����մϴ�\n");
	return 0;
}

// (�۾��ؾ���!!!!!!!!) 2.0 �ֽ� ���� ��ȸ
int allStock(RequestData* req_data, ResponseData* res_data_ptr) {
	printf("\n���� : %d (�ֽ� ���� ��ȸ)\n", req_data->select);
	printf("���� ����: %s\n", req_data->session);
	// ���� ����
	int check;
	check = checkSession(req_data->session);
	// �ֽ� ���� ��ȸ
	res_data_ptr->select = 200;
	if (check) {
		// �ź� ����
		res_data_ptr->check = 1;
		strcpy(res_data_ptr->msg, "������ ��ȸ�� �� �����ϴ�.");
	}
	else {
		res_data_ptr->check = 0;
		// ��� ����
		strcpy(res_data_ptr->msg, "������ ��ȸ�߽��ϴ�.");
		strcpy(res_data_ptr->session, req_data->session);
		init_stock(res_data_ptr);
	}
	return 0;
}
// 2.1 Ŭ���̾�Ʈ ��û - �ֽ� �ż�
int buyStock(RequestData* req_data, ResponseData* res_data_ptr) {
	printf("\n���� : %d (�ֽ� �ż�)\n", req_data->select);
	printf("���� ����: %s\n", req_data->session);
	// ���� ����
	int check, result;
	check = checkSession(req_data->session);
	// �ֽĸż�
	res_data_ptr->select = 201;
	if (check) {
		// �ź� ����
		res_data_ptr->check = 1;
		strcpy(res_data_ptr->msg, "�ż��� �źεǾ����ϴ�.");
	}
	else {
		// ��� ����
		result = db_buyStock(db, req_data->session, req_data->stock_data.stock_id, req_data->stock_data.stock_count);
		if (result) {
			res_data_ptr->check = 1;
			if (result == 1) {
				strcpy(res_data_ptr->msg, "�ż��� �źεǾ����ϴ�.");
			}
			else if (result == 2) {
				strcpy(res_data_ptr->msg, "�ܰ� �����մϴ�.");
			}
			else if (result == 3) {
				strcpy(res_data_ptr->msg, "�� �� �ִ� �ֽ� ������ �ʰ��߽��ϴ�.");
			}
			return 0;
		}
		res_data_ptr->check = 0;
		strcpy(res_data_ptr->session, req_data->session);
		strcpy(res_data_ptr->msg, "�ż��� �Ϸ�Ǿ����ϴ�.");
	}
	return 0;
}

// 2.2 Ŭ���̾�Ʈ ��û - �ֽ� �ŵ�
int sellStock(RequestData* req_data, ResponseData* res_data_ptr) {
	printf("\n���� : %d (�ֽ� �ŵ�)\n", req_data->select);
	printf("���� ����: %s\n", req_data->session);
	// ���� ����
	int check, result;
	char* session = req_data->session;
	check = checkSession(session);
	// �ֽĸŵ�
	res_data_ptr->select = 202;
	if (check) {
		// �ź� ����
		res_data_ptr->check = 1;
		strcpy(res_data_ptr->msg, "�ŵ��� �źεǾ����ϴ�.");
	}
	else {
		// ��� ����
		result = db_sellStock(db, req_data->session, req_data->stock_data.stock_id, req_data->stock_data.stock_count);
		if (result) {
			res_data_ptr->check = 1;
			if (result == 1) {
				strcpy(res_data_ptr->msg, "�ŵ��� �źεǾ����ϴ�.");
			}
			else if (result == 2) {
				strcpy(res_data_ptr->msg, "�ŵ� ������ ���� ���� �������� �����ϴ�.");
			}
			// �ŵ��� �ش� ���� ���ʿ�
			/*else if (result == 3) {
				strcpy(res_data_ptr->msg, "�� �� �ִ� �ֽ� ������ �ʰ��߽��ϴ�.");
			}*/
			return 0;
		}
		res_data_ptr->check = 0;
		strcpy(res_data_ptr->session, req_data->session);
		strcpy(res_data_ptr->msg, "�ŵ��� �Ϸ�Ǿ����ϴ�.");
	}
	return 0;
}

// (�۾��ؾ���!!!!!!!!) 2.3 Ŭ���̾�Ʈ ��û - ������ ��û


