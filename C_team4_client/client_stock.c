#include "client_member.h"
#include "client_stock.h"

delay(clock_t delay_time)
{
	clock_t start = clock(); //clock 함수는 1/1000
	while (clock() - start < delay_time);
	return 0;

}

/**************** 주식 홈 ****************/
// 0. 로그인후 주식관련 홈
stock_home(SOCKET client_fd, char* access) {
	char send_access[MAX_SESSION_LENGTH];
	strcpy(send_access, access);
	do {
		int select;
		printf(">> 주식 서비스 이용 <<\n");
		printf("\n(1.주식 매수 / 2.주식 매도 / 3.로그아웃)\n");
		gotoxy(0, 18);
		printf("\n===================================================================================================================\n\n");
		select = getInputInteger(" 원하는 작업을 지정해주세요 : ");

		system("cls");
		printf(">> 주식 서비스 이용 <<\n\n");

		if(select!=3)
			req_allStock(client_fd, send_access);

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
			printf("\n1, 2, 3번 중 하나를 입력하세요\n\n");
			continue;
		};

	} while (1!=0);

	return 0;
}

/**************** 주식 관련 요청 함수 ****************/
// 1.0 주식 실시간 조회 요청
req_allStock(SOCKET client_fd, char* access) {
	RequestData req_data;
	req_data.select = 200;
	strcpy(req_data.session, access);

	// 서버로 전송
	int bytes_sent = send(client_fd, (RequestData*)&req_data, sizeof(req_data), 0);
	if (bytes_sent == SOCKET_ERROR) {
		fprintf(stderr, "Send failed\n");
		return 1;
	}

	return 0;
}
// 1.1 주식 매수 요청
req_buyStock(SOCKET client_fd, char* access) {
	printf("\n1. 주식 매수\n\n");
	RequestData req_data;
	req_data.select = 201;
	strcpy(req_data.session, access);

	gotoxy(0, 18);
	printf("\n===================================================================================================================\n\n");
	req_data.stock_data.stock_id = getInputInteger("매수할 종목 번호를 입력하세요 : ");
	if (req_data.stock_data.stock_id > 10 || req_data.stock_data.stock_id < 0) {
		req_data.stock_data.stock_id = 100;
	}
	do {
		/*printf("매수할 종목 수량을 입력하세요 : ");
	scanf("%d%*c", &req_data.stock_data.stock_count);*/
		req_data.stock_data.stock_count = getInputInteger("매수할 수량을 입력하세요 : ");
	} while (req_data.stock_data.stock_count < 0);

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
	printf("\n2. 주식 매도\n\n");
	RequestData req_data;
	req_data.select = 202;
	strcpy(req_data.session, access);
	gotoxy(0, 18);
	printf("\n===================================================================================================================\n\n");

	//printf("매도할 종목 번호를 입력하세요 : ");
	//scanf("%d%*c", &req_data.stock_data.stock_id);
	req_data.stock_data.stock_id = getInputInteger("매도할 종목 번호를 입력하세요 : ");
	if (req_data.stock_data.stock_id > 10 || req_data.stock_data.stock_id < 0) {
		req_data.stock_data.stock_id = 100;
	}
	do {
		/*printf("매도할 수량을 입력하세요 : ");
		scanf("%d%*c", &req_data.stock_data.stock_count);*/
		//req_data.stock_data.stock_count = getInputInteger("매도할 수량을 입력하세요: ");
		req_data.stock_data.stock_count = getInputInteger("매도할 수량을 입력하세요 : ");
	} while (req_data.stock_data.stock_count < 0);

	int bytes_sent = send(client_fd, (RequestData*)&req_data, sizeof(req_data), 0);
	if (bytes_sent == SOCKET_ERROR) {
		fprintf(stderr, "Send failed\n");
		return 1;
	}

	WaitForSingleObject(event, INFINITE); // 신호 대기
	return 0;
}
//// 1.3 balance 조회 요청
//req_getBalance(SOCKET client_fd, char* access) {
//	RequestData req_data;
//	req_data.select = 202;
//	strcpy(req_data.session, access);
//	int bytes_sent = send(client_fd, (RequestData*)&req_data, sizeof(req_data), 0);
//	if (bytes_sent == SOCKET_ERROR) {
//		fprintf(stderr, "Send failed\n");
//		return 1;
//	}
//
//	WaitForSingleObject(event, INFINITE); // 신호 대기
//	return 0;
//}

/**************** 주식 관련 리슨 함수 ****************/
// 2.0 주식 정보 조회
res_allStock(ResponseData* res_data) {
	//로그인 직후에는 stock_home보다 먼저 나오고, 기능 작업 후에는 stock_home보다 나중에 나오므로
	//언제든 이전 커서 위치를 기억했다가 복구해야한다.
	delay(100);
	CONSOLE_SCREEN_BUFFER_INFO presentCur; // 콘솔 출력창의 정보를 담기 위해서 정의한 구조체 	
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &presentCur);  //현재 커서의 위치 정보를 저장하는 함수	
	int pre_x = presentCur.dwCursorPosition.X + 1;
	int pre_y = presentCur.dwCursorPosition.Y + 1;

	int y = 1, c = 0;

	// 주식정보 받고 저장
	gotoxy(50, y);
	printf(">> 실시간 주식정보 <<");
	y += 2;
	gotoxy(50, y);	
	printf("[ 주식정보 변경 ]");
	gotoxy(50, ++y);
	printf("=================================================================");
	gotoxy(50, ++y);
	printf("| 종목번호 |        기업명        |  현재가격  |  구매가능수량  |");
	gotoxy(50, ++y);
	printf("=================================================================");
	for (int i = 0; i < MAX_STOCK_RES_LENGTH; i++) {
		gotoxy(50, ++y);
		if (!res_data->stock_arr[i].stock_id) break;
		printf("|  %5d   | %20s | %10d |    %5d       |",
			res_data->stock_arr[i].stock_id, res_data->stock_arr[i].stock_company_name,
			res_data->stock_arr[i].stock_price, res_data->stock_arr[i].stock_count);
	}
	gotoxy(50, ++y);
	printf("=================================================================");

	//원래 좌표로 옮기기!!
	gotoxy(pre_x, pre_y);
	return 0;
}
// 2.1 주식 매수 리슨
res_buyStock(ResponseData* res_data) {
	system("cls");
	printf("%s\n", res_data->msg);
	printf("\n=================================\n\n");
	// 보유 주식 및 잔고 출력

	return 0;
}
// 2.2 주식 매도 리슨
res_sellStock(ResponseData* res_data) {
	system("cls");
	printf("%s\n", res_data->msg);
	printf("\n=================================\n\n");
	// 보유 주식 및 잔고 출력

	return 0;
}
//// 2.3 balance 조회 리슨
//res_getBalance(ResponseData* res_data) {
//	printf("%d\n", res_data->);
//	return 0;
//}
