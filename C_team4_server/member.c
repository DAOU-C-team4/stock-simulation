#include "member.h"
#include "db_connect.h"

sqlite3* db;

// 2.2.1 Ŭ���̾�Ʈ ��û - ȸ�� ����
int add_member(RequestData* req_data) {
	// ȸ�� db ���
	printf("\n���� : %d (ȸ������)\n", req_data->select);
	printf("���� ���̵�: %s", req_data->id);
	printf("���� ��й�ȣ: %s", req_data->password);
	printf("���� �̸�: %s\n", req_data->name);
	insert_member(db, req_data->name, 0, "", req_data->name, req_data->password);
}

// 2.2.2 Ŭ���̾�Ʈ ��û - ȸ�� Ż��
int del_member(RequestData* req_data) {
	// ȸ�� db ����
	printf("\n���� : %d (ȸ��Ż��)\n", req_data->select);
	printf("���� ���̵�: %s", req_data->id);
	printf("���� ��й�ȣ: %s", req_data->password);
}

// 2.2.3 Ŭ���̾�Ʈ ��û - �α���
int login(RequestData* req_data) {
	// �α���
	printf("\n���� : %d (�α���)\n", req_data->select);
	printf("���� ���̵�: %s", req_data->id);
	printf("���� ��й�ȣ: %s", req_data->password);
}

// 2.2.4 Ŭ���̾�Ʈ ��û - �α׾ƿ�
int logout(RequestData* req_data) {
	// �α׾ƿ�
	printf("\n���� : %d (�α׾ƿ�)\n", req_data->select);
}

// 2.2.5 Ŭ���̾�Ʈ ��û - �ֽ� �ż�

// 2.2.6 Ŭ���̾�Ʈ ��û - �ֽ� �ŵ�


// 2.3.1 Ŭ���̾�Ʈ ��û - �ֽ� ���� �ǽð�

// 2.3.2 Ŭ���̾�Ʈ ��û - �ֽ� �Ÿ����� �ǽð�