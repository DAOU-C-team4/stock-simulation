#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")

//#define SERVER_IP "172.30.1.93" // 준하님 pc 서버 ip주소
#define PORT 8080
#define SERVER_IP "127.0.0.1" // 로컬 pc 서버 ip주소
#define MAX_BUFFER_SIZE 1024
#define MAX_ID_LENGTH 50
#define MAX_PASSWORD_LENGTH 50
#define MAX_NAME_LENGTH 50
#define MAX_SESSION_LENGTH 200

// 함수 선언
SOCKET connect_to_server();
select_task_home(SOCKET client_fd);
buyStock(SOCKET client_fd);  // 주식 매수 함수
sellStock(SOCKET client_fd); // 주식 매도 함수
void clear_input_buffer();

// 구조체 선언
typedef struct {
	int stock_id;
	int stock_price;
	int stock_count;
} STOCK_DATA;
typedef struct {
	int select;
	char name[MAX_NAME_LENGTH];
	char id[MAX_ID_LENGTH];
	char password[MAX_PASSWORD_LENGTH];
	char session[MAX_SESSION_LENGTH];
	STOCK_DATA stock_data;
} RequestData;


int main(int argc, char* argv[]) {
	// 0. 소켓연결
	SOCKET client_fd = connect_to_server();

	// 1. 홈화면에서 원하는 작업선택
	select_task_home(client_fd);

	// end. 소켓종료
	closesocket(client_fd);
	WSACleanup();

	return 0;
}

// 0. 소켓연결
SOCKET connect_to_server() {
	WSADATA wsaData;
	SOCKET client_fd;
	struct sockaddr_in server_addr;

	// Initialize Winsock
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		perror("WSAStartup failed");
		exit(EXIT_FAILURE);
	}

	// Create socket
	if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
	server_addr.sin_port = htons(PORT);

	// Connect to server
	if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
		perror("connect failed");
		exit(EXIT_FAILURE);
	}

	printf("Connected to server\n");

	return client_fd;
}

// 1. 홈화면 작업선택 함수
select_task_home(SOCKET client_fd) {
	// 기본 세팅
	int run = 1;
	int isLogin = 0; // 1인 경우 로그인 완료
	char message[MAX_BUFFER_SIZE];
	printf("\n반갑습니다. 키울까말까증권입니다.\n");

	// 홈 메뉴 반복
	do {
		int select = 0;
		printf("\n(1.회원가입 / 2.회원탈퇴 / 3.로그인 / 4.로그아웃 / 5.종료)\n");
		printf("원하는 작업을 지정해주세요 : ");
		scanf("%d%*c", &select);

		printf("\n========================================================================\n");
		switch (select)
		{
		case 1:
			printf("회원가입\n\n");
			add_member(client_fd);
			break;
		case 2:
			printf("회원탈퇴\n\n");
			del_member(client_fd);
			break;
		case 3:
			printf("로그인\n\n");
			//로그인 함수 결과를 isLogin에 저장 (0: 실패, 1: 성공)
			isLogin = login(client_fd);
			//로그인 성공 시, 주식 매매 함수 시작
			if (isLogin) {
				stock_home(client_fd);
			}
			break;
		case 4:
			printf("로그아웃\n\n");
			logout(client_fd);
			isLogin = 0;
			break;
		case 5:
			printf("\n프로그램을 종료합니다. 좋은 하루 되세요 :)\n");
			exit(1);
			break;
		}

		// 서버로부터 수신
		char received_message[MAX_BUFFER_SIZE];
		int bytes_received = recv(client_fd, received_message, MAX_BUFFER_SIZE, 0);
		if (bytes_received == SOCKET_ERROR) {
			perror("recv failed");
			return 1;
		}
		received_message[bytes_received] = '\0'; // 문자열 끝에 NULL 추가
		// 서버로부터 받은 메시지 출력
		printf("%s\n", received_message);

		printf("========================================================================\n");
	} while (run);
}

// 1.1 회원가입요청
add_member(SOCKET client_fd) {
	RequestData req_data;
	req_data.select = 1;

	printf("아이디를 입력하세요: ");
	fgets(req_data.id, MAX_ID_LENGTH, stdin);
	//clear_input_buffer();
	printf("비밀번호를 입력하세요: ");
	fgets(req_data.password, MAX_PASSWORD_LENGTH, stdin);
	//clear_input_buffer();
	printf("이름을 입력하세요: ");
	fgets(req_data.name, MAX_NAME_LENGTH, stdin);
	//clear_input_buffer();

	// 서버로 전송
	int bytes_sent = send(client_fd, (char*)&req_data, sizeof(req_data), 0);
	if (bytes_sent == SOCKET_ERROR) {
		fprintf(stderr, "Send failed\n");
		return 1;
	}
}

// 1.2 회원탈퇴요청
del_member(SOCKET client_fd) {
	RequestData req_data;
	req_data.select = 2;

	printf("아이디를 입력하세요: ");
	fgets(req_data.id, MAX_ID_LENGTH, stdin);
	clear_input_buffer();
	printf("비밀번호를 입력하세요: ");
	fgets(req_data.password, MAX_PASSWORD_LENGTH, stdin);
	clear_input_buffer();

	// 서버로 전송
	int bytes_sent = send(client_fd, (char*)&req_data, sizeof(req_data), 0);
	if (bytes_sent == SOCKET_ERROR) {
		fprintf(stderr, "Send failed\n");
		return 1;
	}
}

// 1.3 로그인
login(SOCKET client_fd) {
	RequestData req_data;
	req_data.select = 3;

	printf("아이디를 입력하세요: ");
	fgets(req_data.id, MAX_ID_LENGTH, stdin);
	clear_input_buffer();
	printf("비밀번호를 입력하세요: ");
	fgets(req_data.password, MAX_PASSWORD_LENGTH, stdin);
	clear_input_buffer();

	// 서버로 전송
	int bytes_sent = send(client_fd, (char*)&req_data, sizeof(req_data), 0);
	if (bytes_sent == SOCKET_ERROR) {
		fprintf(stderr, "Send failed\n");
		return 0;
	}

	return 1;
}

// 1.4 로그아웃
logout(SOCKET client_fd) {
	RequestData req_data;
	req_data.select = 4;

	// 서버로 전송
	int bytes_sent = send(client_fd, (char*)&req_data, sizeof(req_data), 0);
	if (bytes_sent == SOCKET_ERROR) {
		fprintf(stderr, "Send failed\n");
		return 1;
	}
}

// 2 로그인후 주식 관련 홈
stock_home(SOCKET client_fd) {
	do {
		int select = 0;
		printf("\n(1.주식 매수 / 2.주식 매도 / 3.매매 종료)\n");
		printf("원하는 작업을 지정해주세요 : ");
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
			printf("매매 종료\n\n");
			break;
		default:
			printf("\n1, 2, 3번 중 하나를 입력하세요\n");
			break;
		}
	} while (1 != 0);
}

// 입력 버퍼 비우기 함수
void clear_input_buffer() {
	int c;
	while ((c = getchar()) != '\n' && c != EOF);
}

//주식 매수 함수
buyStock(SOCKET client_fd) {
	RequestData req_data;
	req_data.select = 6;
	
	printf("매수할 종목 번호를 입력하세요 : ");
	scanf("%d%*c", &req_data.stock_data.stock_id);
	printf("매수할 금액을 입력하세요 : ");
	scanf("%d%*c", &req_data.stock_data.stock_price);
	printf("매수할 수량을 입력하세요 : ");
	scanf("%d%*c", &req_data.stock_data.stock_count);

	// 서버로 전송
	int bytes_sent = send(client_fd, (RequestData*) &req_data, sizeof(req_data), 0);
	if (bytes_sent == SOCKET_ERROR) {
		fprintf(stderr, "Send failed\n");
		return 1;
	}
}

//주식 매도 함수
sellStock(SOCKET client_fd) {
	RequestData req_data;
	req_data.select = 7;

	printf("매도할 종목 번호를 입력하세요 : ");
	scanf("%d%*c", &req_data.stock_data.stock_id);
	printf("매도할 금액을 입력하세요 : ");
	scanf("%d%*c", &req_data.stock_data.stock_price);
	printf("매도할 수량을 입력하세요 : ");
	scanf("%d%*c", &req_data.stock_data.stock_count);

	int bytes_sent = send(client_fd, (RequestData*) &req_data, sizeof(req_data), 0);
	if (bytes_sent == SOCKET_ERROR) {
		fprintf(stderr, "Send failed\n");
		return 1;
	}
}