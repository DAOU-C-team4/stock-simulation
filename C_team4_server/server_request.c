#include "server_request.h"
#include "server_socket.h"
#include "server_db.h"

sqlite3* db;

/**************** 클라이언트 요청 분기 ****************/
// 2.2 클라이언트 요청 처리 함수
DWORD WINAPI handle_client(SOCKET client_socket) {
	char buffer[MAX_BUFFER_SIZE];
	int bytes_received;
	int run = 1;

	// 클라이언트로부터 요청 대기
	do {
		printf("%d번 클라이언트 요청대기\n", client_socket);
		bytes_received = recv(client_socket, buffer, MAX_BUFFER_SIZE, 0);
		if (bytes_received == SOCKET_ERROR) {
			perror("recv failed");
			return;
		}

		// 클라이언트로부터 받은 데이터를 req_data 구조체로 형변환
		RequestData* req_data = (RequestData*)buffer;
		ResponseData res_data;
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
		}

		// 클라이언트로 결과 전송
		int bytes_sent = send(client_socket, res_data_ptr, sizeof(res_data), 0);
		if (bytes_sent == SOCKET_ERROR) {
			perror("send failed");
			return;
		}
	} while (run);
	return;
}

/**************** 회원 관련 함수 ****************/
// 1.1 클라이언트 요청 - 회원 가입
int add_member(RequestData* req_data, ResponseData* res_data_ptr) {
	// 회원 db 등록
	printf("\n선택 : %d (회원가입)\n", req_data->select);
	printf("받은 아이디: %s", req_data->id);
	printf("받은 비밀번호: %s", req_data->password);
	printf("받은 이름: %s", req_data->name);
	insert_member(db, req_data->name, req_data->id, req_data->password);
	// 응답데이터 기록
	res_data_ptr->select = 1;
	strcpy(res_data_ptr->session, "NONE");
	strcpy(res_data_ptr->msg, "회원가입 완료");
	return 0;
}

// 1.2 클라이언트 요청 - 회원 탈퇴
int del_member(RequestData* req_data, ResponseData* res_data_ptr) {
	// 회원 db 삭제
	printf("\n선택 : %d (회원탈퇴)\n", req_data->select);
	printf("받은 아이디: %s", req_data->id);
	printf("받은 비밀번호: %s", req_data->password);
	delete_member(db, req_data->id, req_data->password);
	// 응답데이터 기록
	res_data_ptr->select = 2;
	strcpy(res_data_ptr->session, "NONE");
	strcpy(res_data_ptr->msg, "회원탈퇴 완료");
	return 0;
}

// 1.3 클라이언트 요청 - 로그인
int login(RequestData* req_data, ResponseData* res_data_ptr) {
	// 로그인
	printf("\n선택 : %d (로그인)\n", req_data->select);
	printf("받은 아이디: %s", req_data->id);
	printf("받은 비밀번호: %s", req_data->password);
	char* access_key;
	access_key = member_login(db, req_data->id, req_data->password);
	// 응답데이터 기록
	res_data_ptr->select = 3;
	strcpy(res_data_ptr->session, access_key);
	strcpy(res_data_ptr->msg, "로그인 완료");
	return 0;
}

// 1.4 클라이언트 요청 - 로그아웃
int logout(RequestData* req_data, ResponseData* res_data_ptr) {
	// 로그아웃
	printf("\n선택 : %d (로그아웃)\n", req_data->select);
	printf("받은 세션: %s\n", req_data->session);
	member_logout(db, req_data->session);
	// 응답데이터 기록
	res_data_ptr->select = 4;
	strcpy(res_data_ptr->session, "CLEAR");
	strcpy(res_data_ptr->msg, "로그아웃 완료");
	return 0;
}


/**************** 주식관련 함수 ****************/
// 2.1 클라이언트 요청 - 주식 매수

// 2.2 클라이언트 요청 - 주식 매도

// 2.3 클라이언트 요청 - 주식 정보 실시간

// 2.4 클라이언트 요청 - 주식 매매정보 실시간