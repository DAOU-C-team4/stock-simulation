#include <stdio.h>
#include <stdlib.h>
#include "client_member.h"

// 로그인후 주식관련 홈
stock_home(SOCKET client_fd) {
	do {
		int select = 0;
		printf("\n(1.주식 매수 / 2.주식 매도 / 3.로그아웃)\n");
		printf("원하는 작업을 지정해주세요 : ");
		scanf("%d%*c", &select);

		printf("\n========================================================================\n");
		switch (select)
		{
		case 1:
			buyStock(client_fd);
			WaitForSingleObject(event, INFINITE); // 신호 대기
			break;
		case 2:
			sellStock(client_fd);
			WaitForSingleObject(event, INFINITE); // 신호 대기
			break;
		case 3:
			printf("매매 종료\n");
			return;
		default:
			printf("\n1, 2, 3번 중 하나를 입력하세요\n");
			continue;
		}
	} while (1 != 0);
}

//주식 매수 함수
buyStock(SOCKET client_fd) {
	RequestData req_data;
	req_data.select = 6;

	printf("매수할 종목 번호를 입력하세요 : ");
	scanf("%d%*c", &req_data.stock_data.stock_id);
	printf("매수할 수량을 입력하세요 : ");
	scanf("%d%*c", &req_data.stock_data.stock_count);

	// 서버로 전송
	int bytes_sent = send(client_fd, (RequestData*)&req_data, sizeof(req_data), 0);
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
	printf("매도할 수량을 입력하세요 : ");
	scanf("%d%*c", &req_data.stock_data.stock_count);

	int bytes_sent = send(client_fd, (RequestData*)&req_data, sizeof(req_data), 0);
	if (bytes_sent == SOCKET_ERROR) {
		fprintf(stderr, "Send failed\n");
		return 1;
	}
}