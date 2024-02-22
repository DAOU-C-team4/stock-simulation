#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"

// 콜백 함수 - SELECT 쿼리 결과를 처리하는 함수
static int callback(void* NotUsed, int argc, char** argv, char** azColName) {
	int i;
	for (i = 0; i < argc; i++) {
		printf("%s: %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}

// 1.1 데이터베이스 연결 함수
sqlite3* open_database(const char* db_name) {
	sqlite3* db;
	int rc = sqlite3_open(db_name, &db);
	if (rc) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		return NULL;
	}
	else {
		fprintf(stderr, "Opened database successfully\n");
		return db;
	}
}

// 1.2 회원 테이블 생성 함수
int create_member_table(sqlite3* db) {
	char* zErrMsg = 0;
	const char* sql_create_table =
		"CREATE TABLE MEMBER(" \
		"ID INTEGER PRIMARY KEY AUTOINCREMENT," \
		"NAME TEXT NOT NULL," \
		"AGE INTEGER," \
		"EMAIL TEXT," \
		"USER_ID TEXT UNIQUE NOT NULL," \
		"PASSWORD TEXT NOT NULL);";
	int rc = sqlite3_exec(db, sql_create_table, callback, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return 0;
	}
	else {
		fprintf(stdout, "Member table created successfully\n");
		return 1;
	}
}

// 1.3 회원 정보 추가 함수
int insert_member(sqlite3* db, const char* name, int age, const char* email, const char* user_id, const char* password) {
	char* zErrMsg = 0;
	char sql_insert_member[500];
	sprintf(sql_insert_member, "INSERT INTO MEMBER (NAME, AGE, EMAIL, USER_ID, PASSWORD) VALUES ('%s', %d, '%s', '%s', '%s');",
		name, age, email ? email : "", user_id, password);
	int rc = sqlite3_exec(db, sql_insert_member, callback, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return 0;
	}
	else {
		fprintf(stdout, "Member added successfully\n");
		return 1;
	}
}

// 1.4 회원 정보 삭제 함수

// 1.5 회원 로그인 함수

// 1.6 회원 로그아웃 함수
