#include <stdio.h>
#include <stdlib.h>
#include "client_member.h"

/**************** 주식 홈 ****************/
// 0. 로그인후 주식관련 홈
stock_home(SOCKET client_fd, char* access) {
	char send_access[MAX_SESSION_LENGTH];
	strcpy(send_access, access);
	do {
		int select = 0;
		printf("\n(1.주식 매수 / 2.주식 매도 / 3.로그아웃)\n");
		printf("원하는 작업을 지정해주세요 : ");
		scanf("%d%*c", &select);

		printf("\n========================================================================\n\n");
		switch (select)
		{
		case 1:
			req_buyStock(client_fd, send_access);
			break;
		case 2:
			req_sellStock(client_fd, send_access);
			break;
		case 3:
			printf("매매 종료. 안녕히가세요 :)\n");
			return;
		default:
			printf("\n1, 2, 3번 중 하나를 입력하세요\n");
			continue;
		}
	} while (1 != 0);

	return 0;
}

/**************** 주식 관련 요청 함수 ****************/
// 1.1 주식 매수 요청
req_buyStock(SOCKET client_fd, char* access) {
	printf("주식 매수\n\n");
	RequestData req_data;
	req_data.select = 201;
	strcpy(req_data.session, access);

	printf("매수할 종목 번호를 입력하세요 : ");
	scanf("%d%*c", &req_data.stock_data.stock_id);
	do {
		printf("매수할 수량을 입력하세요: ");
		scanf("%d%*c", &req_data.stock_data.stock_count);
	} while (req_data.stock_data.stock_count < 0);

	// 서버로 전송
	int bytes_sent = send(client_fd, (RequestData*)&req_data, sizeof(req_data), 0);
	if (bytes_sent == SOCKET_ERROR) {
		fprintf(stderr, "Send failed\n");
		return 1;
	}

	WaitForSingleObject(event, INFINITE); // 신호 대기
	return 0;
}

// 1.2 주식 매도 요청
req_sellStock(SOCKET client_fd, char* access) {
	printf("주식 매도\n\n");
	RequestData req_data;
	req_data.select = 202;
	strcpy(req_data.session, access);


	printf("매도할 종목 번호를 입력하세요 : ");
	scanf("%d%*c", &req_data.stock_data.stock_id);
	do {
		printf("매도할 수량을 입력하세요: ");
		scanf("%d%*c", &req_data.stock_data.stock_count);
	} while (req_data.stock_data.stock_count < 0);

	int bytes_sent = send(client_fd, (RequestData*)&req_data, sizeof(req_data), 0);
	if (bytes_sent == SOCKET_ERROR) {
		fprintf(stderr, "Send failed\n");
		return 1;
	}

	WaitForSingleObject(event, INFINITE); // 신호 대기
	return 0;
}

/**************** 주식 관련 리슨 함수 ****************/
// 2.0 주식 정보 조회
res_allStock(ResponseData* res_data) {
	// 주식정보 받고 저장
	printf("주식정보 변경\n");

	return 0;
}
// 2.1 주식 매수 리슨
res_buyStock(ResponseData* res_data) {
	printf("주식매수\n");
	printf("%s\n", res_data->msg);
	// 보유 주식 및 잔고 출력

	return 0;
}
// 2.2 주식 매도 리슨
res_sellStock(ResponseData* res_data) {
	printf("주식매도\n");
	printf("%s\n", res_data->msg);
	// 보유 주식 및 잔고 출력

	return 0;
}