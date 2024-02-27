#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include "client_socket.h"
#include "client_member.h"
#include "client_stock.h"



/**************** ȸ�� ���� ��û �Լ� ****************/
// 1.1 ȸ������ ��û
req_add_member(SOCKET client_fd) {
	//system("cls");
	//printf("\n�ȳ��ϼ���. Ű���������Դϴ�.\n");
	printf(">> ȸ������ <<\n\n");
	RequestData req_data;
	req_data.select = 1;
	char password_check[MAX_PASSWORD_LENGTH];

	// ���̵� �Է�
	char* user_id = getInputStringLimited("���̵� �Է��ϼ���: ", MAX_NAME_LENGTH);
	strcpy(req_data.id, user_id);
	free(user_id);
	// ��й�ȣ �Է�
	enterPassword(req_data.password);
	enterPasswordCheck(password_check);
	// ��й�ȣ Ȯ��
	if (strcmp(req_data.password, password_check) != 0) {
		printf("\n��й�ȣ�� ��ġ���� �ʽ��ϴ�. �ٽ� �õ����ּ���.\n");
		return 1;
	}
	// �̸� �Է�
	char* name = getInputStringLimited("�̸��� �Է��ϼ���: ", MAX_NAME_LENGTH);
	strcpy(req_data.name, name);
	free(name);

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

	// ���̵� �Է�
	char* user_id = getInputStringLimited("���̵� �Է��ϼ���: ", MAX_NAME_LENGTH);
	strcpy(req_data.id, user_id);
	free(user_id);
	// ��й�ȣ �Է�
	enterPasswordLogin(req_data.password);
	// ����
	char check[MAX_BUFFER_SIZE];
	printf("\n���� Ż���Ͻðڽ��ϱ�? (y/n): ");
	fgets(check, sizeof(check), stdin);
	// ���ڿ��� ���� �ִ� ���� ���ڸ� �����մϴ�.
	check[strcspn(check, "\n")] = '\0';
	// �Է��� "y"�� ��ġ�ϴ��� Ȯ���մϴ�.
	if (strcmp(check, "y") == 0 || strcmp(check, "Y") == 0) {
		// Ż�� ó���� �����մϴ�.
	}
	else if (strcmp(check, "n") == 0 || strcmp(check, "N") == 0) {
		printf("ȸ��Ż�� ��ҵǾ����ϴ�.\n");
		return 0; // Ż�� ���
	}
	else {
		printf("�߸��� �Է��Դϴ�.\n");
		return 0; // �߸��� �Է�
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

	// ���̵� �Է�
	char* user_id = getInputStringLimited("���̵� �Է��ϼ���: ", MAX_NAME_LENGTH);
	strcpy(req_data.id, user_id);
	free(user_id);
	// ��й�ȣ �Է�
	enterPasswordLogin(req_data.password);

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

	printf("��й�ȣ�� �Է��ϼ��� (�ּ� %d�� �̻�, ����, Ư������ ����): ", MIN_PASSWORD_LENGTH);
	while (1) {
		char ch = getch(); // ����ڰ� �Է��� ���ڸ� �о�� (ȭ�鿡 ǥ�õ��� ����)

		// ����Ű�� �ԷµǸ� �Է� ����
		if (ch == '\r' || ch == '\n') {
			if (i < MIN_PASSWORD_LENGTH) {
				printf("\n��й�ȣ�� �ʹ� ª���ϴ�. �ٽ� �Է��ϼ��� : ");
				i = 0;
				continue;
			}
			if (!hasAlpha(password, i)) {
				printf("\n��й�ȣ�� ���ڰ� ���ԵǾ� ���� �ʽ��ϴ�. �ٽ� �Է��ϼ��� : ");
				i = 0;
				continue;
			}
			if (!hasSpecialChar(password, i)) {
				printf("\n��й�ȣ�� Ư�����ڰ� ���ԵǾ� ���� �ʽ��ϴ�. �ٽ� �Է��ϼ��� : ");
				i = 0;
				continue;
			}
			if (!hasDigit(password, i)) {
				printf("\n��й�ȣ�� ���ڰ� ���ԵǾ� ���� �ʽ��ϴ�. �ٽ� �Է��ϼ��� : ");
				i = 0;
				continue;
			}
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
	printf("\n");
	return 0;
}

// �н����� Ȯ�� ��ȣó�� �Լ�
enterPasswordCheck(char* password) {
	int i = 0;
	printf("��й�ȣ�� �ѹ� �� �Է��ϼ��� : ");
	while (1) {
		char ch = getch(); // ����ڰ� �Է��� ���ڸ� �о�� (ȭ�鿡 ǥ�õ��� ����)

		// ����Ű�� �ԷµǸ� �Է� ����
		if (ch == '\r' || ch == '\n') {
			if (i == 0) {
				printf("\n��й�ȣ�� �ѹ� �� �Է��ϼ��� : ");
				continue;
			}
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
	printf("\n");
	return 0;
}

// �н����� �Է��Լ�
enterPasswordLogin(char* password) {
	int i = 0;
	printf("��й�ȣ�� �Է��ϼ���: ");
	while (1) {
		char ch = getch(); // ����ڰ� �Է��� ���ڸ� �о�� (ȭ�鿡 ǥ�õ��� ����)

		// ����Ű�� �ԷµǸ� �Է� ����
		if (ch == '\r' || ch == '\n') {
			if (i == 0) {
				printf("\n��й�ȣ�� �Է��ϼ���: ");
				continue;
			}
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

		// �Է¿��� ����߽߰� �ƿ�ó��
		for (int i = 0; i < strlen(input); i++) {
			if (isspace(input[i]) && i != strlen(input) - 1 || isalpha(input[i])) {
				strcpy(input, "byebye");
				break;
			}
		}
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

// ���ڿ� �Է� �Լ�
char* getInputStringLimited(char* prompt, int max_length) {
	char* input = (char*)malloc(max_length * sizeof(char)); // �Է¹��� ���ڿ��� �������� �Ҵ�
	if (input == NULL) {
		printf("�޸� �Ҵ� ����\n");
		exit(EXIT_FAILURE);
	}

	printf("%s", prompt);
	fgets(input, max_length, stdin);
	// ���� ���� ����
	input[strcspn(input, "\n")] = '\0';

	// ���ڿ��� �ִ� ���̸� �ʰ��ϰų� �� ���� ���
	while (strlen(input) >= max_length - 1 || strlen(input) == 0) {
		if (strlen(input) == 0) {
			printf("�� ���� �Է��� �� �����ϴ�. �ٽ� �Է��ϼ���.\n");
		}
		else {
			printf("�Է��� ���ڿ��� �ʹ� ��ϴ�. �ٽ� �Է��ϼ���.\n");
			// �Է� ���� ����
			int c;
			while ((c = getchar()) != '\n' && c != EOF);
		}
		printf("%s", prompt);
		fgets(input, max_length, stdin);
		// ���� ���� ����
		input[strcspn(input, "\n")] = '\0';
	}

	return input;
}

// Ư������ ���� ���θ� Ȯ���ϴ� �Լ�
bool hasSpecialChar(const char* password, int i) {
	for (int j = 0; j < i; j++) {
		if (!isalnum(*password) && !isspace(*password)) {
			return true; // Ư�����ڰ� ���ԵǾ� ����
		}
		password++;
	}
	return false; // Ư�����ڰ� ���ԵǾ� ���� ����
}

// ���� ���� ���θ� Ȯ���ϴ� �Լ�
bool hasDigit(const char* password, int i) {
	for (int j = 0; j < i; j++) {
		if (isdigit(*password)) {
			return true; // ���ڰ� ���ԵǾ� ����
		}
		password++;
	}
	return false; // ���ڰ� ���ԵǾ� ���� ����
}

// ���ĺ� ���� ���θ� Ȯ���ϴ� �Լ�
bool hasAlpha(const char* password, int i) {
	for (int j = 0; j < i; j++) {
		if (isalpha(password[j])) {
			return true; // ���ĺ��� ���ԵǾ� ����
		}
	}
	return false; // ���ĺ��� ���ԵǾ� ���� ����
}