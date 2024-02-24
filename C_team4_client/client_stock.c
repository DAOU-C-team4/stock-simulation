#include <stdio.h>
#include <stdlib.h>
#include "client_member.h"

// 로그인후 주식관련 홈
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
			printf("매매 종료\n");
			return;
		default:
			printf("\n1, 2, 3번 중 하나를 입력하세요\n");
			continue;
		}

		// 서버로부터 수신
		char received_message[MAX_BUFFER_SIZE];
		int bytes_received = recv(client_fd, received_message, MAX_BUFFER_SIZE, 0);
		if (bytes_received == SOCKET_ERROR) {
			perror("recv failed");
			return 1;
		}
		// 서버로부터 받은 메시지 출력
		ResponseData* Res_data = (ResponseData*)received_message;
		
		printf("\n서버로부터 응답:\n");
		printf("========================================================================\n");
		printf("%s\n", Res_data->msg);
		//printf("   종목번호: %d\n", Res_data->select);
		//printf("   종목이름: %s\n", Res_data->stock_name);
		//printf("   매입가격: %d\n", Res_data->stock_price);
		//printf("   수   량: %d\n", Res_data->stock_count);
		printf("========================================================================\n");

	} while (1 != 0);
}

//주식 매수 요청 함수
buyStock(SOCKET client_fd) {
	RequestData Req_data;
	Req_data.select = 5; //1. 회원가입, 2. 회원탈퇴, 3. 로그인, 4. 로그아웃, 5. 매매
	Req_data.stock_data.select = 1; // 매매 분기 => 매수: 1, 매도 : 2 

	printf("매수할 종목 번호를 입력하세요 : ");
	scanf("%d%*c", &(Req_data.stock_data.stock_id));
	printf("매수할 수량을 입력하세요 : ");
	scanf("%d%*c", &(Req_data.stock_data.stock_count));

	// 서버로 전송, 
	int bytes_sent = send(client_fd, (RequestData*)&Req_data, sizeof(Req_data), 0);
	if (bytes_sent == SOCKET_ERROR) {
		fprintf(stderr, "Send failed\n");
		return 1;
	}
}

//주식 매도 요청 함수
sellStock(SOCKET client_fd) {
	RequestData Req_data;
	Req_data.select = 5; //1. 회원가입, 2. 회원탈퇴, 3. 로그인, 4. 로그아웃, 5. 매매
	Req_data.stock_data.select = 2; // 매매 분기 => 매수: 1, 매도 : 2 

	printf("매도할 종목 번호를 입력하세요 : ");
	scanf("%d%*c", &(Req_data.stock_data.stock_id));
	printf("매도할 수량을 입력하세요 : ");
	scanf("%d%*c", &(Req_data.stock_data.stock_count));

	// 서버로 전송, 
	int bytes_sent = send(client_fd, (RequestData*)&Req_data, sizeof(Req_data), 0);
	if (bytes_sent == SOCKET_ERROR) {
		fprintf(stderr, "Send failed\n");
		return 1;
	}
}