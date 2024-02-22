#include "member.h"
#include "db_connect.h"

sqlite3* db;

// 2.2.1 클라이언트 요청 - 회원 가입
int add_member(RequestData* req_data) {
	// 회원 db 등록
	printf("\n선택 : %d (회원가입)\n", req_data->select);
	printf("받은 아이디: %s", req_data->id);
	printf("받은 비밀번호: %s", req_data->password);
	printf("받은 이름: %s\n", req_data->name);
	insert_member(db, req_data->name, 0, "", req_data->name, req_data->password);
}

// 2.2.2 클라이언트 요청 - 회원 탈퇴
int del_member(RequestData* req_data) {
	// 회원 db 삭제
	printf("\n선택 : %d (회원탈퇴)\n", req_data->select);
	printf("받은 아이디: %s", req_data->id);
	printf("받은 비밀번호: %s", req_data->password);
}

// 2.2.3 클라이언트 요청 - 로그인
int login(RequestData* req_data) {
	// 로그인
	printf("\n선택 : %d (로그인)\n", req_data->select);
	printf("받은 아이디: %s", req_data->id);
	printf("받은 비밀번호: %s", req_data->password);
}

// 2.2.4 클라이언트 요청 - 로그아웃
int logout(RequestData* req_data) {
	// 로그아웃
	printf("\n선택 : %d (로그아웃)\n", req_data->select);
}

// 2.2.5 클라이언트 요청 - 주식 매수

// 2.2.6 클라이언트 요청 - 주식 매도


// 2.3.1 클라이언트 요청 - 주식 정보 실시간

// 2.3.2 클라이언트 요청 - 주식 매매정보 실시간