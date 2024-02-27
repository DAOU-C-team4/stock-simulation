#include "server_request.h"
#include "server_socket.h"
#include "server_db.h"

static int init_stock(ResponseData* res_data_ptr) {
	STOCK_RES* result = db_allStock(db);
	if (result) {
		int stock_arr_size = sizeof(res_data_ptr->stock_arr) / sizeof(res_data_ptr->stock_arr[0]);

		for (int i = 0; i < stock_arr_size; i++) {
			/*printf("stock_id[%d] = %d\n", i, result[i].stock_id);
			printf("stock_name[%d] = %d\n", i, result[i].stock_name);
			printf("stock_company_name[%d] = %s\n", i, result[i].stock_company_name);
			printf("stock_price[%d] = %d\n", i, result[i].stock_price);
			printf("stock_count[%d] = %d\n\n", i, result[i].stock_count);*/

			res_data_ptr->stock_arr[i].stock_id = result[i].stock_id;
			res_data_ptr->stock_arr[i].stock_name = result[i].stock_name;
			strcpy(res_data_ptr->stock_arr[i].stock_company_name, result[i].stock_company_name);
			res_data_ptr->stock_arr[i].stock_price = result[i].stock_price;
			res_data_ptr->stock_arr[i].stock_count = result[i].stock_count;
		}
		return 0;
	}
}

/**************** 클라이언트 요청 분기 ****************/
// 2.2 클라이언트 요청 처리 함수
DWORD WINAPI handle_client(SOCKET client_socket) {
	char buffer[MAX_BUFFER_SIZE];
	int bytes_received, bytes_sent;
	int run = 1;

	// 클라이언트로부터 요청 대기
	do {
		printf("%d번 클라이언트 요청대기\n", client_socket);
		bytes_received = recv(client_socket, buffer, MAX_BUFFER_SIZE, 0);
		if (bytes_received == SOCKET_ERROR) {
			perror("recv failed");
			for (int i = 0; i < FD_SETSIZE; i++) {
				if (client_sockets[i] == client_socket) {
					client_tf[i] = 0;
				}
			}
			return 1;
		}

		// 클라이언트로부터 받은 데이터를 req_data 구조체로 형변환
		RequestData* req_data = (RequestData*)buffer;
		ResponseData res_data = { 0 };
		ResponseData* res_data_ptr = &res_data;

		// 요청에 따라 다른 작업 실행
		switch (req_data->select)
		{
		case 1:
			add_member(req_data, res_data_ptr);
			break;
		case 2:
			del_member(req_data, res_data_ptr);
			break;
		case 3:
			login(req_data, res_data_ptr);
			break;
		case 4:
			logout(req_data, res_data_ptr);
			break;
		case 5:
			// 회원 정보 조회 요청
			break;
		case 200:
			allStock(req_data, res_data_ptr);
			break;
		case 201:
			buyStock(req_data, res_data_ptr);
			bytes_sent = send(client_socket, res_data_ptr, sizeof(res_data), 0);
			if (bytes_sent == SOCKET_ERROR) {
				perror("send buyStock() failed");
				return 1;
			}
			sendAllClnt(res_data_ptr);
			break;
		case 202:
			sellStock(req_data, res_data_ptr);
			bytes_sent = send(client_socket, res_data_ptr, sizeof(res_data), 0);
			if (bytes_sent == SOCKET_ERROR) {
				perror("send sellStock() failed");
				return 1;
			}
			sendAllClnt(res_data_ptr);
			break;
		default:
			// 잘못된 요청 처리
			break;
		}

		// 클라이언트로 결과 전송
		bytes_sent = send(client_socket, res_data_ptr, sizeof(res_data), 0);
		if (bytes_sent == SOCKET_ERROR) {
			perror("send failed");
			return 1;
		}
	} while (run);
	return 0;
}

/**************** 회원 관련 함수 ****************/
// 1.1 클라이언트 요청 - 회원 가입
int add_member(RequestData* req_data, ResponseData* res_data_ptr) {
	// 회원 db 등록
	printf("\n선택 : %d (회원가입)\n", req_data->select);
	printf("받은 아이디: %s\n", req_data->id);
	printf("받은 비밀번호: %s\n", req_data->password);
	printf("받은 이름: %s\n", req_data->name);
	// 응답데이터 기록
	int result;
	result = db_insert_member(db, req_data->name, req_data->id, req_data->password);
	res_data_ptr->select = 1;
	strcpy(res_data_ptr->session, "NONE");
	if (result) {
		strcpy(res_data_ptr->msg, "회원가입 완료");
	}
	else {
		strcpy(res_data_ptr->msg, "회원가입 실패");
	}
	return 0;
}

// 1.2 클라이언트 요청 - 회원 탈퇴
int del_member(RequestData* req_data, ResponseData* res_data_ptr) {
	// 회원 db 삭제
	printf("\n선택 : %d (회원탈퇴)\n", req_data->select);
	printf("받은 아이디: %s\n", req_data->id);
	printf("받은 비밀번호: %s\n", req_data->password);
	// 응답데이터 기록
	int result;
	result = db_delete_member(db, req_data->id, req_data->password);
	res_data_ptr->select = 2;
	strcpy(res_data_ptr->session, "NONE");
	if (result) {
		strcpy(res_data_ptr->msg, "회원탈퇴 완료");
	}
	else {
		strcpy(res_data_ptr->msg, "회원탈퇴 실패. 아이디 비밀번호를 확인해주세요.");
	}
	return 0;
}

// 1.3 클라이언트 요청 - 로그인
int login(RequestData* req_data, ResponseData* res_data_ptr) {
	// 로그인
	printf("\n선택 : %d (로그인)\n", req_data->select);
	printf("받은 아이디: %s\n", req_data->id);
	printf("받은 비밀번호: %s\n", req_data->password);
	char* access_key;
	access_key = db_login(db, req_data->id, req_data->password);
	// 응답데이터 기록
	res_data_ptr->select = 3;
	strcpy(res_data_ptr->session, access_key);
	if (!strcmp(access_key, "NONE")) {
		strcpy(res_data_ptr->msg, "로그인 실패. 아이디 비밀번호를 확인해주세요.");
	}
	else if (access_key == NULL) {
		strcpy(res_data_ptr->msg, "서버 에러");
	}
	else {
		strcpy(res_data_ptr->msg, "로그인 성공");
	}

	init_stock(res_data_ptr);
	return 0;
}

// 1.4 클라이언트 요청 - 로그아웃
int logout(RequestData* req_data, ResponseData* res_data_ptr) {
	// 로그아웃
	printf("\n선택 : %d (로그아웃)\n", req_data->select);
	printf("받은 세션: %s\n", req_data->session);
	db_logout(db, req_data->session);
	// 응답데이터 기록
	res_data_ptr->select = 4;
	strcpy(res_data_ptr->session, "CLEAR");
	strcpy(res_data_ptr->msg, "로그아웃 완료");
	return 0;
}


/**************** 주식관련 함수 ****************/

// **클라이언트 요청 - 전체 회원에게 보내기
int sendAllClnt(ResponseData* res_data_ptr) {
	res_data_ptr->select = 200;
	init_stock(res_data_ptr);
	int bytes_sent;
	// 클라이언트로 결과 전송
	for (int i = 0; i < FD_SETSIZE; i++) {
		if (client_tf[i] == 1)
			bytes_sent = send(client_sockets[i], res_data_ptr, sizeof(*res_data_ptr), 0);
		if (bytes_sent == SOCKET_ERROR) {
			perror("send failed : ");
			return 1;
		}
	}
	return 0;
}

// (작업해야함!!!!!!!!) 2.0 세션 유효 검증
int checkSession(char* session) {
	printf("세션을 검증합니다\n");
	return 0;
}

// (작업해야함!!!!!!!!) 2.0 주식 정보 조회
int allStock(RequestData* req_data, ResponseData* res_data_ptr) {
	printf("\n선택 : %d (주식 정보 조회)\n", req_data->select);
	printf("받은 세션: %s\n", req_data->session);
	// 세션 검증
	int check;
	check = checkSession(req_data->session);
	// 주식 정보 조회
	res_data_ptr->select = 200;
	if (check) {
		// 거부 응답
		res_data_ptr->check = 1;
		strcpy(res_data_ptr->msg, "정보를 조회할 수 없습니다.");
	}
	else {
		res_data_ptr->check = 0;
		// 허락 응답
		strcpy(res_data_ptr->msg, "정보를 조회했습니다.");
		strcpy(res_data_ptr->session, req_data->session);
		init_stock(res_data_ptr);
	}
	return 0;
}
// 2.1 클라이언트 요청 - 주식 매수
int buyStock(RequestData* req_data, ResponseData* res_data_ptr) {
	printf("\n선택 : %d (주식 매수)\n", req_data->select);
	printf("받은 세션: %s\n", req_data->session);
	// 세션 검증
	int check, result;
	check = checkSession(req_data->session);
	// 주식매수
	res_data_ptr->select = 201;
	if (check) {
		// 거부 응답
		res_data_ptr->check = 1;
		strcpy(res_data_ptr->msg, "매수가 거부되었습니다.");
	}
	else {
		// 허락 응답
		result = db_buyStock(db, req_data->session, req_data->stock_data.stock_id, req_data->stock_data.stock_count);
		if (result) {
			res_data_ptr->check = 1;
			if (result == 1) {
				strcpy(res_data_ptr->msg, "매수가 거부되었습니다.");
			}
			else if (result == 2) {
				strcpy(res_data_ptr->msg, "잔고가 부족합니다.");
			}
			else if (result == 3) {
				strcpy(res_data_ptr->msg, "살 수 있는 주식 갯수를 초과했습니다.");
			}
			return 0;
		}
		res_data_ptr->check = 0;
		strcpy(res_data_ptr->session, req_data->session);
		strcpy(res_data_ptr->msg, "매수가 완료되었습니다.");
	}
	return 0;
}

// 2.2 클라이언트 요청 - 주식 매도
int sellStock(RequestData* req_data, ResponseData* res_data_ptr) {
	printf("\n선택 : %d (주식 매도)\n", req_data->select);
	printf("받은 세션: %s\n", req_data->session);
	// 세션 검증
	int check, result;
	char* session = req_data->session;
	check = checkSession(session);
	// 주식매도
	res_data_ptr->select = 202;
	if (check) {
		// 거부 응답
		res_data_ptr->check = 1;
		strcpy(res_data_ptr->msg, "매도가 거부되었습니다.");
	}
	else {
		// 허락 응답
		result = db_sellStock(db, req_data->session, req_data->stock_data.stock_id, req_data->stock_data.stock_count);
		if (result) {
			res_data_ptr->check = 1;
			if (result == 1) {
				strcpy(res_data_ptr->msg, "매도가 거부되었습니다.");
			}
			else if (result == 2) {
				strcpy(res_data_ptr->msg, "매도 수량이 현재 보유 수량보다 많습니다.");
			}
			// 매도는 해당 내용 불필요
			/*else if (result == 3) {
				strcpy(res_data_ptr->msg, "살 수 있는 주식 갯수를 초과했습니다.");
			}*/
			return 0;
		}
		res_data_ptr->check = 0;
		strcpy(res_data_ptr->session, req_data->session);
		strcpy(res_data_ptr->msg, "매도가 완료되었습니다.");
	}
	return 0;
}

// (작업해야함!!!!!!!!) 2.3 클라이언트 요청 - 내정보 요청


