#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include "client_socket.h"
#include "client_member.h"
#include "client_stock.h"



/**************** ȸ�� ���� ��û �Լ� ****************/
// 1.1 ȸ������ ��û
req_add_member(SOCKET client_fd) {
	printf(">> ȸ������ <<\n\n");
	RequestData req_data;
	req_data.select = 1;
	char password_check[MAX_PASSWORD_LENGTH];

	printf("���̵� �Է��ϼ���: ");
	fgets(req_data.id, MAX_ID_LENGTH, stdin);
	printf("��й�ȣ�� �Է��ϼ���: ");
	enterPassword(req_data.password);
	printf("\n��й�ȣ�� �ѹ��� �Է��ϼ���: ");
	enterPassword(password_check);
	// ��й�ȣ Ȯ��
	if (strcmp(req_data.password, password_check) != 0) {
		printf("\n��й�ȣ�� ��ġ���� �ʽ��ϴ�. �ٽ� �õ����ּ���.\n");
		return 1;
	}
	printf("\n�̸��� �Է��ϼ���: ");
	fgets(req_data.name, MAX_NAME_LENGTH, stdin);

	// ������ ����
	int bytes_sent = send(client_fd, (char*)&req_data, sizeof(req_data), 0);
	if (bytes_sent == SOCKET_ERROR) {
		fprintf(stderr, "Send failed\n");
		return 1;
	}

	WaitForSingleObject(event, INFINITE); // ��ȣ ���
	return 0;
}

// 1.2 ȸ��Ż�� ��û
req_del_member(SOCKET client_fd) {
	//system("cls");
	//printf("\n�ȳ��ϼ���. Ű���������Դϴ�.\n");
	printf(">> ȸ��Ż�� <<\n\n");
	RequestData req_data;
	req_data.select = 2;
	char confirmation = 'y';

	printf("���̵� �Է��ϼ���: ");
	fgets(req_data.id, MAX_ID_LENGTH, stdin);
	printf("��й�ȣ�� �Է��ϼ���: ");
	enterPassword(req_data.password);
	//fgets(req_data.password, MAX_PASSWORD_LENGTH, stdin);
	printf("\n���� Ż���Ͻðڽ��ϱ�? (y/n): ");
	confirmation = fgetc(stdin);
	if (confirmation != 'y' && confirmation != 'Y') {
		printf("ȸ��Ż�� ��ҵǾ����ϴ�.\n");
		return 0; // Ż�� ���
	}

	// ������ ����
	int bytes_sent = send(client_fd, (char*)&req_data, sizeof(req_data), 0);
	if (bytes_sent == SOCKET_ERROR) {
		fprintf(stderr, "Send failed\n");
		return 1;
	}

	WaitForSingleObject(event, INFINITE); // ��ȣ ���
	return 0;
}

// 1.3 �α��� ��û
req_login(SOCKET client_fd) {
	//system("cls");
	//printf("\n�ݰ����ϴ�. Ű���������Դϴ�.\n");
	printf(">> �α��� <<\n\n");
	RequestData req_data;
	req_data.select = 3;

	printf("���̵� �Է��ϼ���: ");
	fgets(req_data.id, MAX_ID_LENGTH, stdin);
	printf("��й�ȣ�� �Է��ϼ���: ");
	enterPassword(req_data.password);
	//fgets(req_data.password, MAX_PASSWORD_LENGTH, stdin);

	// ������ ����
	int bytes_sent = send(client_fd, (char*)&req_data, sizeof(req_data), 0);
	if (bytes_sent == SOCKET_ERROR) {
		fprintf(stderr, "Send failed\n");
		return 1;
	}

	WaitForSingleObject(event, INFINITE); // ��ȣ ���
	return 0;
}

// 1.4 �α׾ƿ� ��û
req_logout(SOCKET client_fd, char* access) {
	RequestData req_data;
	req_data.select = 4;
	strcpy(req_data.session, access);

	// ������ ����
	int bytes_sent = send(client_fd, (char*)&req_data, sizeof(req_data), 0);
	if (bytes_sent == SOCKET_ERROR) {
		fprintf(stderr, "Send failed\n");
		return 1;
	}

	WaitForSingleObject(event, INFINITE); // ��ȣ ���
	return 0;
}

/**************** ȸ�� ���� ���� �Լ� ****************/
// 2.1 ȸ������ ����
res_add_member(ResponseData* res_data) {
	//system("cls");
	//printf(">> ȸ������ <<\n\n");
	system("cls");
	printf("%s\n", res_data->msg);
	return 0;
}

// 2.2 ȸ��Ż�� ����
res_del_member(ResponseData* res_data) {
	//system("cls");
	//printf(">> ȸ��Ż�� <<\n\n");
	system("cls");
	printf("%s\n", res_data->msg);
	return 0;
}

// 2.3 �α��� ����
res_login(ResponseData* res_data, char* access) {
	//system("cls");
	//printf(">> �α��� <<\n\n");
	system("cls");
	printf("%s\n", res_data->msg);
	printf("   session: %s\n", res_data->session);
	strcpy(access, res_data->session);
	//res_allStock(res_data);
	return 0;
}

// 2.4 �α׾ƿ� ����
res_logout(ResponseData* res_data, char* access) {
	printf(">> �α׾ƿ� <<\n\n");
	printf("   session: %s\n", res_data->session);
	strcpy(access, "NONE");
	return 0;
}

// 2.5 ȸ������ ����
res_memberInfo(ResponseData* res_data, char* access) {
	printf(">> ȸ������ <<\n\n");

	return 0;
}

/**************** ��Ÿ �Է� �Լ� ****************/
// �ܼ� ����� �Լ�
clearConsoleArea(int left, int top, int width, int height) {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	// ���� �ܼ� â�� ���� ���� ��������
	GetConsoleScreenBufferInfo(hConsole, &csbi);

	DWORD dwConSize = width * height;
	COORD upperLeft = { (SHORT)left, (SHORT)top };
	DWORD dwCharsWritten;

	// Ư�� ������ �������� ä���
	FillConsoleOutputCharacter(hConsole, TEXT(' '), dwConSize, upperLeft, &dwCharsWritten);
	FillConsoleOutputAttribute(hConsole, csbi.wAttributes, dwConSize, upperLeft, &dwCharsWritten);

	// Ŀ�� ��ġ ����
	SetConsoleCursorPosition(hConsole, upperLeft);
}

// �н����� ��ȣó�� �Լ�
enterPassword(char* password) {
	int i = 0;
	while (1) {
		char ch = getch(); // ����ڰ� �Է��� ���ڸ� �о�� (ȭ�鿡 ǥ�õ��� ����)

		// ����Ű�� �ԷµǸ� �Է� ����
		if (ch == '\r' || ch == '\n') {
			password[i] = '\0'; // ��й�ȣ ���ڿ� ���� null ���� �߰�
			break;
		}
		// �齺���̽� Ű�� �ԷµǸ� ���� ���� ����
		else if (ch == '\b' && i > 0) {
			i--;
			printf("\b \b"); // ȭ�鿡�� �齺���̽��� ������ ���� �����
		}
		// ��й�ȣ �Է� �� '*' ���ڷ� ����ŷ�Ͽ� ���
		else if (i < MAX_PASSWORD_LENGTH - 1) {
			password[i++] = ch;
			printf("*");
		}
	}
	return 0;
}

// ���� �Է� �Լ�
int getInputInteger(char* prompt) {
	char input[20]; // ����� ���� �Ҵ�
	int value;
	bool validInput = false;

	while (!validInput) {
		printf("%s", prompt);
		fgets(input, sizeof(input), stdin);
		// ���ڿ��� ������ ��ȯ
		if (sscanf(input, "%d", &value) != 1) {
			printf("�ùٸ� ������ �Է��ϼ���.\n");
			continue;
		}
		// �Է°��� ���� Ȯ��
		if (value < INT_MIN || value > INT_MAX) {
			printf("�Է°��� ������ ����ϴ�.\n");
			continue;
		}
		validInput = true;
	}

	return value;
}