#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include "client_socket.h"
#include "client_member.h"
#include "client_stock.h"



/**************** 회원 관련 요청 함수 ****************/
// 1.1 회원가입 요청
req_add_member(SOCKET client_fd) {
	//system("cls");
	//printf("\n안녕하세요. 키울까말까증권입니다.\n");
	printf(">> 회원가입 <<\n\n");
	RequestData req_data;
	req_data.select = 1;
	char password_check[MAX_PASSWORD_LENGTH];

	// 아이디 입력
	char* user_id = getInputStringLimited("아이디를 입력하세요: ", MAX_NAME_LENGTH);
	strcpy(req_data.id, user_id);
	free(user_id);
	// 비밀번호 입력
	enterPassword(req_data.password);
	enterPasswordCheck(password_check);
	// 비밀번호 확인
	if (strcmp(req_data.password, password_check) != 0) {
		printf("\n비밀번호가 일치하지 않습니다. 다시 시도해주세요.\n");
		return 1;
	}
	// 이름 입력
	char* name = getInputStringLimited("이름을 입력하세요: ", MAX_NAME_LENGTH);
	strcpy(req_data.name, name);
	free(name);

	// 서버로 전송
	int bytes_sent = send(client_fd, (char*)&req_data, sizeof(req_data), 0);
	if (bytes_sent == SOCKET_ERROR) {
		fprintf(stderr, "Send failed\n");
		return 1;
	}

	WaitForSingleObject(event, INFINITE); // 신호 대기
	return 0;
}

// 1.2 회원탈퇴 요청
req_del_member(SOCKET client_fd) {
	//system("cls");
	//printf("\n안녕하세요. 키울까말까증권입니다.\n");
	printf(">> 회원탈퇴 <<\n\n");
	RequestData req_data;
	req_data.select = 2;
	char confirmation = 'y';

	// 아이디 입력
	char* user_id = getInputStringLimited("아이디를 입력하세요: ", MAX_NAME_LENGTH);
	strcpy(req_data.id, user_id);
	free(user_id);
	// 비밀번호 입력
	enterPasswordLogin(req_data.password);
	// 검증
	char check[MAX_BUFFER_SIZE];
	printf("\n정말 탈퇴하시겠습니까? (y/n): ");
	fgets(check, sizeof(check), stdin);
	// 문자열의 끝에 있는 개행 문자를 제거합니다.
	check[strcspn(check, "\n")] = '\0';
	// 입력이 "y"와 일치하는지 확인합니다.
	if (strcmp(check, "y") == 0 || strcmp(check, "Y") == 0) {
		// 탈퇴 처리를 진행합니다.
	}
	else if (strcmp(check, "n") == 0 || strcmp(check, "N") == 0) {
		printf("회원탈퇴가 취소되었습니다.\n");
		return 0; // 탈퇴 취소
	}
	else {
		printf("잘못된 입력입니다.\n");
		return 0; // 잘못된 입력
	}

	// 서버로 전송
	int bytes_sent = send(client_fd, (char*)&req_data, sizeof(req_data), 0);
	if (bytes_sent == SOCKET_ERROR) {
		fprintf(stderr, "Send failed\n");
		return 1;
	}

	WaitForSingleObject(event, INFINITE); // 신호 대기
	return 0;
}

// 1.3 로그인 요청
req_login(SOCKET client_fd) {
	//system("cls");
	//printf("\n반갑습니다. 키울까말까증권입니다.\n");
	printf(">> 로그인 <<\n\n");
	RequestData req_data;
	req_data.select = 3;

	// 아이디 입력
	char* user_id = getInputStringLimited("아이디를 입력하세요: ", MAX_NAME_LENGTH);
	strcpy(req_data.id, user_id);
	free(user_id);
	// 비밀번호 입력
	enterPasswordLogin(req_data.password);

	// 서버로 전송
	int bytes_sent = send(client_fd, (char*)&req_data, sizeof(req_data), 0);
	if (bytes_sent == SOCKET_ERROR) {
		fprintf(stderr, "Send failed\n");
		return 1;
	}

	WaitForSingleObject(event, INFINITE); // 신호 대기
	return 0;
}

// 1.4 로그아웃 요청
req_logout(SOCKET client_fd, char* access) {
	RequestData req_data;
	req_data.select = 4;
	strcpy(req_data.session, access);

	// 서버로 전송
	int bytes_sent = send(client_fd, (char*)&req_data, sizeof(req_data), 0);
	if (bytes_sent == SOCKET_ERROR) {
		fprintf(stderr, "Send failed\n");
		return 1;
	}

	WaitForSingleObject(event, INFINITE); // 신호 대기
	return 0;
}

/**************** 회원 관련 리슨 함수 ****************/
// 2.1 회원가입 리슨
res_add_member(ResponseData* res_data) {
	//system("cls");
	//printf(">> 회원가입 <<\n\n");
	system("cls");
	printf("%s\n", res_data->msg);
	return 0;
}

// 2.2 회원탈퇴 리슨
res_del_member(ResponseData* res_data) {
	//system("cls");
	//printf(">> 회원탈퇴 <<\n\n");
	system("cls");
	printf("%s\n", res_data->msg);
	return 0;
}

// 2.3 로그인 리슨
res_login(ResponseData* res_data, char* access) {
	//system("cls");
	//printf(">> 로그인 <<\n\n");
	system("cls");
	printf("%s\n", res_data->msg);
	printf("   session: %s\n", res_data->session);
	strcpy(access, res_data->session);
	//res_allStock(res_data);
	return 0;
}

// 2.4 로그아웃 리슨
res_logout(ResponseData* res_data, char* access) {
	printf(">> 로그아웃 <<\n\n");
	printf("   session: %s\n", res_data->session);
	strcpy(access, "NONE");
	return 0;
}

// 2.5 회원정보 리슨
res_memberInfo(ResponseData* res_data, char* access) {
	printf(">> 회원정보 <<\n\n");

	return 0;
}

/**************** 기타 입력 함수 ****************/
// 콘솔 지우기 함수
clearConsoleArea(int left, int top, int width, int height) {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	// 현재 콘솔 창의 버퍼 정보 가져오기
	GetConsoleScreenBufferInfo(hConsole, &csbi);

	DWORD dwConSize = width * height;
	COORD upperLeft = { (SHORT)left, (SHORT)top };
	DWORD dwCharsWritten;

	// 특정 영역을 공백으로 채우기
	FillConsoleOutputCharacter(hConsole, TEXT(' '), dwConSize, upperLeft, &dwCharsWritten);
	FillConsoleOutputAttribute(hConsole, csbi.wAttributes, dwConSize, upperLeft, &dwCharsWritten);

	// 커서 위치 조정
	SetConsoleCursorPosition(hConsole, upperLeft);
}

// 패스워드 암호처리 함수
enterPassword(char* password) {
	int i = 0;

	printf("비밀번호를 입력하세요 (최소 %d자 이상, 숫자, 특수문자 포함): ", MIN_PASSWORD_LENGTH);
	while (1) {
		char ch = getch(); // 사용자가 입력한 문자를 읽어옴 (화면에 표시되지 않음)

		// 엔터키가 입력되면 입력 종료
		if (ch == '\r' || ch == '\n') {
			if (i < MIN_PASSWORD_LENGTH) {
				printf("\n비밀번호가 너무 짧습니다. 다시 입력하세요 : ");
				i = 0;
				continue;
			}
			if (!hasAlpha(password, i)) {
				printf("\n비밀번호에 문자가 포함되어 있지 않습니다. 다시 입력하세요 : ");
				i = 0;
				continue;
			}
			if (!hasSpecialChar(password, i)) {
				printf("\n비밀번호에 특수문자가 포함되어 있지 않습니다. 다시 입력하세요 : ");
				i = 0;
				continue;
			}
			if (!hasDigit(password, i)) {
				printf("\n비밀번호에 숫자가 포함되어 있지 않습니다. 다시 입력하세요 : ");
				i = 0;
				continue;
			}
			password[i] = '\0'; // 비밀번호 문자열 끝에 null 문자 추가
			break;
		}
		// 백스페이스 키가 입력되면 이전 문자 삭제
		else if (ch == '\b' && i > 0) {
			i--;
			printf("\b \b"); // 화면에서 백스페이스로 삭제된 문자 지우기
		}
		// 비밀번호 입력 시 '*' 문자로 마스킹하여 출력
		else if (i < MAX_PASSWORD_LENGTH - 1) {
			password[i++] = ch;
			printf("*");
		}
	}
	printf("\n");
	return 0;
}

// 패스워드 확인 암호처리 함수
enterPasswordCheck(char* password) {
	int i = 0;
	printf("비밀번호를 한번 더 입력하세요 : ");
	while (1) {
		char ch = getch(); // 사용자가 입력한 문자를 읽어옴 (화면에 표시되지 않음)

		// 엔터키가 입력되면 입력 종료
		if (ch == '\r' || ch == '\n') {
			if (i == 0) {
				printf("\n비밀번호를 한번 더 입력하세요 : ");
				continue;
			}
			password[i] = '\0'; // 비밀번호 문자열 끝에 null 문자 추가
			break;
		}
		// 백스페이스 키가 입력되면 이전 문자 삭제
		else if (ch == '\b' && i > 0) {
			i--;
			printf("\b \b"); // 화면에서 백스페이스로 삭제된 문자 지우기
		}
		// 비밀번호 입력 시 '*' 문자로 마스킹하여 출력
		else if (i < MAX_PASSWORD_LENGTH - 1) {
			password[i++] = ch;
			printf("*");
		}
	}
	printf("\n");
	return 0;
}

// 패스워드 입력함수
enterPasswordLogin(char* password) {
	int i = 0;
	printf("비밀번호를 입력하세요: ");
	while (1) {
		char ch = getch(); // 사용자가 입력한 문자를 읽어옴 (화면에 표시되지 않음)

		// 엔터키가 입력되면 입력 종료
		if (ch == '\r' || ch == '\n') {
			if (i == 0) {
				printf("\n비밀번호를 입력하세요: ");
				continue;
			}
			password[i] = '\0'; // 비밀번호 문자열 끝에 null 문자 추가
			break;
		}
		// 백스페이스 키가 입력되면 이전 문자 삭제
		else if (ch == '\b' && i > 0) {
			i--;
			printf("\b \b"); // 화면에서 백스페이스로 삭제된 문자 지우기
		}
		// 비밀번호 입력 시 '*' 문자로 마스킹하여 출력
		else if (i < MAX_PASSWORD_LENGTH - 1) {
			password[i++] = ch;
			printf("*");
		}
	}
	return 0;
}

// 정수 입력 함수
int getInputInteger(char* prompt) {
	char input[20]; // 충분한 공간 할당
	int value;
	bool validInput = false;

	while (!validInput) {
		printf("%s", prompt);
		fgets(input, sizeof(input), stdin);

		// 입력에서 공백발견시 아웃처리
		for (int i = 0; i < strlen(input); i++) {
			if (isspace(input[i]) && i != strlen(input) - 1 || isalpha(input[i])) {
				strcpy(input, "byebye");
				break;
			}
		}
		// 문자열을 정수로 변환
		if (sscanf(input, "%d", &value) != 1) {
			printf("올바른 정수를 입력하세요.\n");
			continue;
		}
		// 입력값의 범위 확인
		if (value < INT_MIN || value > INT_MAX) {
			printf("입력값이 범위를 벗어납니다.\n");
			continue;
		}
		validInput = true;
	}

	return value;
}

// 문자열 입력 함수
char* getInputStringLimited(char* prompt, int max_length) {
	char* input = (char*)malloc(max_length * sizeof(char)); // 입력받을 문자열을 동적으로 할당
	if (input == NULL) {
		printf("메모리 할당 오류\n");
		exit(EXIT_FAILURE);
	}

	printf("%s", prompt);
	fgets(input, max_length, stdin);
	// 개행 문자 제거
	input[strcspn(input, "\n")] = '\0';

	// 문자열이 최대 길이를 초과하거나 빈 값인 경우
	while (strlen(input) >= max_length - 1 || strlen(input) == 0) {
		if (strlen(input) == 0) {
			printf("빈 값을 입력할 수 없습니다. 다시 입력하세요.\n");
		}
		else {
			printf("입력한 문자열이 너무 깁니다. 다시 입력하세요.\n");
			// 입력 버퍼 비우기
			int c;
			while ((c = getchar()) != '\n' && c != EOF);
		}
		printf("%s", prompt);
		fgets(input, max_length, stdin);
		// 개행 문자 제거
		input[strcspn(input, "\n")] = '\0';
	}

	return input;
}

// 특수문자 포함 여부를 확인하는 함수
bool hasSpecialChar(const char* password, int i) {
	for (int j = 0; j < i; j++) {
		if (!isalnum(*password) && !isspace(*password)) {
			return true; // 특수문자가 포함되어 있음
		}
		password++;
	}
	return false; // 특수문자가 포함되어 있지 않음
}

// 숫자 포함 여부를 확인하는 함수
bool hasDigit(const char* password, int i) {
	for (int j = 0; j < i; j++) {
		if (isdigit(*password)) {
			return true; // 숫자가 포함되어 있음
		}
		password++;
	}
	return false; // 숫자가 포함되어 있지 않음
}

// 알파벳 포함 여부를 확인하는 함수
bool hasAlpha(const char* password, int i) {
	for (int j = 0; j < i; j++) {
		if (isalpha(password[j])) {
			return true; // 알파벳이 포함되어 있음
		}
	}
	return false; // 알파벳이 포함되어 있지 않음
}