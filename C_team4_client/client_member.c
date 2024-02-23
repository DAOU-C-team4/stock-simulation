#include <stdio.h>
#include <stdlib.h>
#include "client_socket.h"

// 1.1 ȸ�����Կ�û
add_member(SOCKET client_fd) {
	RequestData req_data;
	req_data.select = 1;

	printf("���̵� �Է��ϼ���: ");
	fgets(req_data.id, MAX_ID_LENGTH, stdin);
	printf("��й�ȣ�� �Է��ϼ���: ");
	fgets(req_data.password, MAX_PASSWORD_LENGTH, stdin);
	printf("�̸��� �Է��ϼ���: ");
	fgets(req_data.name, MAX_NAME_LENGTH, stdin);

	// ������ ����
	int bytes_sent = send(client_fd, (char*)&req_data, sizeof(req_data), 0);
	if (bytes_sent == SOCKET_ERROR) {
		fprintf(stderr, "Send failed\n");
		return 1;
	}
}

// 1.2 ȸ��Ż���û
del_member(SOCKET client_fd) {
	RequestData req_data;
	req_data.select = 2;

	printf("���̵� �Է��ϼ���: ");
	fgets(req_data.id, MAX_ID_LENGTH, stdin);
	printf("��й�ȣ�� �Է��ϼ���: ");
	fgets(req_data.password, MAX_PASSWORD_LENGTH, stdin);

	// ������ ����
	int bytes_sent = send(client_fd, (char*)&req_data, sizeof(req_data), 0);
	if (bytes_sent == SOCKET_ERROR) {
		fprintf(stderr, "Send failed\n");
		return 1;
	}
}

// 1.3 �α���
login(SOCKET client_fd) {
	RequestData req_data;
	req_data.select = 3;

	printf("���̵� �Է��ϼ���: ");
	fgets(req_data.id, MAX_ID_LENGTH, stdin);
	printf("��й�ȣ�� �Է��ϼ���: ");
	fgets(req_data.password, MAX_PASSWORD_LENGTH, stdin);

	// ������ ����
	int bytes_sent = send(client_fd, (char*)&req_data, sizeof(req_data), 0);
	if (bytes_sent == SOCKET_ERROR) {
		fprintf(stderr, "Send failed\n");
		return 0;
	}

	return 1;
}

// 1.4 �α׾ƿ�
logout(SOCKET client_fd, char* access) {
	RequestData req_data;
	req_data.select = 4;
	strcpy(req_data.session, access);
	strcpy(access, "NONE");

	// ������ ����
	int bytes_sent = send(client_fd, (char*)&req_data, sizeof(req_data), 0);
	if (bytes_sent == SOCKET_ERROR) {
		fprintf(stderr, "Send failed\n");
		return 1;
	}
}