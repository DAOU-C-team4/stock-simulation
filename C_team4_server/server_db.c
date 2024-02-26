#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"
#include "server_request.h"

// 주식 id 파싱
matching_data[10] = { 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000 };

int idx = 0;

/**************** DB 연결 기본 ****************/
// 콜백 함수 1 - SELECT 쿼리 결과를 처리하는 함수
static int callback(void* NotUsed, int argc, char** argv, char** azColName) {
	int i;
	for (i = 0; i < argc; i++) {
		printf("%s: %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}
// 콜백 함수 2 - 로그인 쿼리 결과를 처리하는 함수
static int callback_login(void* user_count_ptr, int argc, char** argv, char** azColName) {
	int* user_count = (int*)user_count_ptr;
	*user_count = atoi(argv[0]); // 첫 번째 열 값을 정수로 변환하여 user_count에 저장
	printf("Callback called: user count = %d\n", *user_count);
	return 0;
}
// 콜백 함수 3 - 주식 가격 로드
static int callback_stock_price(void* user_price_ptr, int argc, char** argv, char** azColName) {
	int* price = (int*)user_price_ptr;
	*price = atoi(argv[0]); // 첫 번째 열 값을 정수로 변환하여 저장
	return 0;
}
// 콜백 함수 4 - 주식 갯수 로드
static int callback_stock_count(void* user_count_ptr, int argc, char** argv, char** azColName) {
	int* count = (int*)user_count_ptr;
	*count = atoi(argv[0]); // 첫 번째 열 값을 정수로 변환하여 저장
	return 0;
}
// 콜백 함수 5 - 사용자 잔액 조회
static int callback_stock_balance(void* user_balance_ptr, int argc, char** argv, char** azColName) {
	int* balance = (int*)user_balance_ptr;
	*balance = atoi(argv[0]); // 첫 번째 열 값을 정수로 변환하여 저장
	return 0;
}
//콜백함수 6 - 전체 주식 조회
static int callback_stockinfo(void* stockinfo_ptr, int argc, char** argv, char** azColName) {
	STOCK_RES* stockinfo = (STOCK_RES*)stockinfo_ptr;
	for (int i = 0; i < argc; i++){
		stockinfo[idx].stock_id = atoi(argv[0]); // 첫 번째 열 값을 정수로 변환하여 저장
		stockinfo[idx].stock_name = atoi(argv[1]);
		strcpy(stockinfo[idx].stock_company_name, argv[2]);
		stockinfo[idx].stock_price = atoi(argv[3]);
		stockinfo[idx].stock_count = atoi(argv[4]);
		//printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	idx++;
	return 0;
}


// 0. 데이터베이스 연결 함수
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

/**************** 테이블 관련 함수 ****************/
// 1.1 회원 테이블 생성 함수
int create_member_table(sqlite3* db) {
	char* zErrMsg = 0;
	const char* sql_create_table =
		"CREATE TABLE MEMBER(" \
		"ID INTEGER PRIMARY KEY AUTOINCREMENT," \
		"NAME TEXT NOT NULL," \
		"USER_ID TEXT UNIQUE NOT NULL," \
		"PASSWORD TEXT NOT NULL," \
		"BALANCE INTEGER NOT NULL DEFAULT 5000000 CHECK (BALANCE >= 0),"\
		"ACCESS_KEY TEXT);";
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

// 1.2 주식 테이블 생성함수
int create_stock_table(sqlite3* db) {
	char* zErrMsg = 0;
	const char* sql_create_table =
		"CREATE TABLE STOCK(" \
		"ID INTEGER PRIMARY KEY AUTOINCREMENT," \
		"MATCH_ID INTEGER UNIQUE NOT NULL," \
		"COMPANY_NAME TEXT UNIQUE NOT NULL," \
		"CURRENT_PRICE INTEGER NOT NULL," \
		"ORIGINAL_PRICE INTEGER NOT NULL," \
		"STOCK_COUNT INTEGER NOT NULL CHECK (STOCK_COUNT >= 0));";
	int rc = sqlite3_exec(db, sql_create_table, callback, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return 0;
	}
	else {
		fprintf(stdout, "Stock table created successfully\n");
		return 1;
	}
}

// 1.3 주식잔고 테이블 생성함수
int create_account_table(sqlite3* db) {
	char* zErrMsg = 0;
	const char* sql_create_table =
		"CREATE TABLE ACCOUNT(" \
		"ID INTEGER PRIMARY KEY AUTOINCREMENT," \
		"MEMBER_ID INTEGER NOT NULL," \
		"STOCK_ID INTEGER NOT NULL," \
		"COMPANY_NAME TEXT NOT NULL," \
		"QUANTITY INTEGER NOT NULL," \
		"PURCHASE_PRICE INTEGER NOT NULL," \
		"FOREIGN KEY(MEMBER_ID) REFERENCES MEMBER(ID)," \
		"FOREIGN KEY(STOCK_ID) REFERENCES STOCK(ID));"; // 외부 키 정의
	int rc = sqlite3_exec(db, sql_create_table, callback, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return 0;
	}
	else {
		fprintf(stdout, "Account table created successfully\n");
		return 1;
	}
}

int insert_stock_dummyData(sqlite3* db) {
	char* zErrMsg = 0;
	const char* sql_insert_data =
		"INSERT INTO STOCK (MATCH_ID, COMPANY_NAME, CURRENT_PRICE, ORIGINAL_PRICE, STOCK_COUNT) "
		"VALUES "
		"(100, 'Daou', 22000, 9800, 2000), "
		"(200, 'Kiwoom Securities', 126400, 33000, 1500), "
		"(300, 'Samsung Electronics', 72900, 4350000, 1800), "
		"(400, 'SK Hynix', 161400, 1170000, 2500), "
		"(500, 'LG Energy Solution', 402000, 940000, 5800), "
		"(600, 'Samsung BioLogics', 814000, 570000, 2300), "
		"(700, 'Hyundai Motor', 244000, 510000, 1900), "
		"(800, 'Kia', 118400, 470000, 2600), "
		"(900, 'Celltrion', 180000, 390000, 3900), "
		"(1000, 'EcoproBM', 247500, 240000, 1400);";
	int rc = sqlite3_exec(db, sql_insert_data, callback, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return 0;
	}
	else {
		fprintf(stdout, "Data inserted successfully\n");
		return 1;
	}
}

/**************** 회원 관련 DB 함수 ****************/
// 2.1 회원 정보 추가 함수
int db_insert_member(sqlite3* db, const char* name, const char* user_id, const char* password) {
	char* zErrMsg = 0;
	char sql_insert_member[500];
	sprintf(sql_insert_member, "INSERT INTO MEMBER (NAME, USER_ID, PASSWORD) VALUES ('%s', '%s', '%s');",
		name, user_id, password);
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

// 2.2 회원 정보 삭제 함수
int db_delete_member(sqlite3* db, const char* user_id, const char* password) {
	char* zErrMsg = 0;
	char sql_delete_member[200];
	sprintf(sql_delete_member, "DELETE FROM MEMBER WHERE USER_ID = '%s' AND PASSWORD = '%s';", user_id, password);
	int rc = sqlite3_exec(db, sql_delete_member, callback, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return 0;
	}
	else {
		fprintf(stdout, "Member deleted successfully\n");
		return 1;
	}
}

// 2.3 회원 로그인 함수
int db_login(sqlite3* db, const char* user_id, const char* password) {
	// 유효한 사용자인지 확인
	if (!is_valid_user(db, user_id, password)) {
		fprintf(stderr, "Invalid user credentials\n");
		return "NONE"; // 유효하지 않은 사용자인 경우 NULL 반환
	}

	// 랜덤키 생성 및 검증
	char access_key[50];
	int is_duplicate = 0;
	do {
		generate_random_key(access_key);
		is_duplicate = is_duplicate_key(db, access_key);
		if (is_duplicate == -1) {
			return NULL;
		}
	} while (is_duplicate > 0);

	// 에세스키 db등록
	char* zErrMsg = 0;
	char sql_update_access_key[200];
	sprintf(sql_update_access_key, "UPDATE MEMBER SET ACCESS_KEY = '%s' WHERE USER_ID = '%s' AND PASSWORD = '%s';", access_key, user_id, password);
	int rc = sqlite3_exec(db, sql_update_access_key, NULL, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return NULL;
	}

	// 에세스키 리턴
	return access_key;
}

// 2.3.0 사용자가 유효한지 확인하는 함수
int is_valid_user(sqlite3* db, const char* user_id, const char* password) {
	char* zErrMsg = 0;
	char sql_query[200];
	sprintf(sql_query, "SELECT COUNT(*) FROM MEMBER WHERE USER_ID = '%s' AND PASSWORD = '%s';", user_id, password);
	int user_count = 0;
	int rc = sqlite3_exec(db, sql_query, callback_login, &user_count, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return 0; // 오류가 발생한 경우 유효하지 않은 사용자로 처리
	}
	printf("%d\n", user_count);
	return user_count; // 사용자가 존재하면 1, 그렇지 않으면 0 반환
}

// 2.3.1 랜덤 에세스키 생성 함수
int generate_random_key(char* key) {
	const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	srand((unsigned int)time(NULL)); // 현재 시간을 시드로 사용
	for (int i = 0; i < 30; i++) { // 길이가 30인 고정된 문자열 생성
		int index = rand() % strlen(charset);
		key[i] = charset[index];
	}
	key[30] = '\0'; // NULL 문자('\0') 추가
	return 0;
}

// 2.3.2 에세스 키가 데이터베이스에 중복되는지 검사하는 함수
int is_duplicate_key(sqlite3* db, const char* key) {
	char* zErrMsg = 0;
	char sql_check_duplicate_key[200];
	sprintf(sql_check_duplicate_key, "SELECT COUNT(*) FROM MEMBER WHERE ACCESS_KEY = '%s';", key);
	int key_count = 0;
	int rc = sqlite3_exec(db, sql_check_duplicate_key, callback, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return -1; // 오류 발생 시 -1 반환
	}
	return key_count; // 중복된 키가 있으면 1 이상의 값 반환, 없으면 0 반환
}

// 2.4 회원 로그아웃 함수
int db_logout(sqlite3* db, const char* session) {
	// 여기서는 단순히 로그아웃 메시지를 출력하는 것으로 가정합니다.
	char* zErrMsg = 0;
	char sql_delete_member[200];
	sprintf(sql_delete_member, "UPDATE MEMBER SET ACCESS_KEY = '' WHERE ACCESS_KEY = '%s';", session);
	int rc = sqlite3_exec(db, sql_delete_member, callback, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return 0;
	}
	else {
		fprintf(stdout, "Member logged out successfully\n");
		return 1;
	}
}

/**************** 주식 관련 DB 함수 ****************/
// (작업해야함!!!!!!!!) 3.0 주식 정보조회
STOCK_RES* db_allStock(sqlite3* db) {
	char* zErrMsg = 0;
	char sql_select_stock[200];
	sprintf(sql_select_stock, "SELECT ID, MATCH_ID, COMPANY_NAME, CURRENT_PRICE, STOCK_COUNT FROM STOCK;");
	STOCK_RES stockinfo[MAX_STOCK_RES_LENGTH] = { 0 };
	idx = 0;
	int rc = sqlite3_exec(db, sql_select_stock, callback_stockinfo, stockinfo, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return stockinfo; // 오류가 발생한 경우 유효하지 않은 사용자로 처리
	}
	/*for (int i = 0; i < 10; i++) {
		printf("stock_id[%d] = %d\n", i, stockinfo[i].stock_id);
		printf("stock_name[%d] = %d\n", i, stockinfo[i].stock_name);
		printf("stock_company_name[%d] = %s\n", i, stockinfo[i].stock_company_name);
		printf("stock_price[%d] = %d\n", i, stockinfo[i].stock_price);
		printf("stock_count[%d] = %d\n", i, stockinfo[i].stock_count);
	}*/
	printf("\n");
	return stockinfo; // 구조체 반환
}

// 3.1 주식 매수
int db_buyStock(sqlite3* db, char* session, int s_id, int s_cnt) {
	char* zErrMsg = 0;
	char sql_buyStock[500];
	int stock_price = 0, stock_count = 0, balance = 0;
	printf("%d 주식 %d개 매수", matching_data[s_id - 1], s_cnt);

	// 주식 가격 및 가격 조회
	char sql_query[200];
	sprintf(sql_query, "SELECT CURRENT_PRICE FROM STOCK WHERE MATCH_ID = '%d';", matching_data[s_id - 1]);
	int rc = sqlite3_exec(db, sql_query, callback_stock_price, &stock_price, &zErrMsg);
	if (rc != SQLITE_OK) {
		sqlite3_free(zErrMsg);
		return 1;
	}
	sprintf(sql_query, "SELECT STOCK_COUNT FROM STOCK WHERE MATCH_ID = '%d';", matching_data[s_id - 1]);
	rc = sqlite3_exec(db, sql_query, callback_stock_count, &stock_count, &zErrMsg);
	if (rc != SQLITE_OK) {
		sqlite3_free(zErrMsg);
		return 1;
	}
	sprintf(sql_query, "SELECT BALANCE FROM MEMBER WHERE ACCESS_KEY = '%s';", session);
	rc = sqlite3_exec(db, sql_query, callback_stock_balance, &balance, &zErrMsg);
	if (rc != SQLITE_OK) {
		sqlite3_free(zErrMsg);
		return 1;
	}
	printf("받아온 주식 가격 : %d, 남은갯수 : %d, 잔고 : %d, 세션 : %s\n", stock_price, stock_count, balance, session);

	// 돈이 모자를시
	if (stock_price * s_cnt > balance) {
		fprintf(stderr, "잔액이 부족합니다.\n");
		return 2;
	}
	// 주식 갯수가 모자를시
	if (s_cnt > stock_count) {
		fprintf(stderr, "주식 갯수가 부족합니다.\n");
		return 3;
	}

	// 회원 테이블 - 회원 예치금 변경
	sprintf(sql_buyStock, "UPDATE MEMBER SET BALANCE = BALANCE - %d WHERE ACCESS_KEY = '%s';", stock_price * (int)s_cnt, session);
	rc = sqlite3_exec(db, sql_buyStock, callback, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		sqlite3_free(zErrMsg);
		return 1;
	}
	fprintf(stdout, "Update User Money successfully\n");
	// 주식 테이블 - 주식 정보 변경
	// (작업해야함!!!!!!!!) 주식 가격 변동로직
	sprintf(sql_buyStock, "UPDATE STOCK SET STOCK_COUNT = STOCK_COUNT - %d WHERE MATCH_ID = '%d';", (int)s_cnt, matching_data[s_id - 1]);
	rc = sqlite3_exec(db, sql_buyStock, callback, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		sqlite3_free(zErrMsg);
		return 1;
	}
	fprintf(stdout, "Buy Stock successfully\n");
	// (작업해야함!!!!!!!!)내역 테이블 - 주식 보유 내역에 기록
	sprintf(sql_buyStock, ";");
	rc = sqlite3_exec(db, sql_buyStock, callback, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		sqlite3_free(zErrMsg);
		return 1;
	}

	return 0;
}

// 3.2 주식 매도
int db_sellStock(sqlite3* db, char* session, int s_id, int s_cnt) {
	char* zErrMsg = 0;
	char sql_buyStock[500];
	int stock_price = 0, stock_count = 0;
	printf("%d 주식 %d개 매도", matching_data[s_id - 1], s_cnt);
	// 주식 가격 및 가격 조회
	char sql_query[200];
	sprintf(sql_query, "SELECT CURRENT_PRICE FROM STOCK WHERE MATCH_ID = '%d';", matching_data[s_id - 1]);
	int rc = sqlite3_exec(db, sql_query, callback_stock_price, &stock_price, &zErrMsg);
	if (rc != SQLITE_OK) {
		sqlite3_free(zErrMsg);
		return 1;
	}
	// (작업해야함!!!!!!!!) 회원이 가진 주식갯수 불러오기 

	printf("받아온 주식 가격 : %d, 가진갯수 : %d\n", stock_price, stock_count);
	// (작업해야함!!!!!!!!) 회원이 가진 주식갯수와 비교

	// 회원 테이블 - 회원 예치금 변경
	sprintf(sql_buyStock, "UPDATE MEMBER SET BALANCE = BALANCE + %d WHERE ACCESS_KEY = '%s';", stock_price * (int)s_cnt, session);
	rc = sqlite3_exec(db, sql_buyStock, callback, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		sqlite3_free(zErrMsg);
		return 1;
	}
	fprintf(stdout, "Update User Money successfully\n");
	// 주식 테이블 - 주식 정보 변경
	sprintf(sql_buyStock, "UPDATE STOCK SET STOCK_COUNT = STOCK_COUNT + %d WHERE MATCH_ID = '%d';", (int)s_cnt, matching_data[s_id - 1]);
	rc = sqlite3_exec(db, sql_buyStock, callback, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		sqlite3_free(zErrMsg);
		return 1;
	}
	fprintf(stdout, "Sell Stock successfully\n");
	// (작업해야함!!!!!!!!)내역 테이블 - 주식 보유 내역에 기록

	return 0;
}

// (작업해야함!!!!!!!!) 3.3 내 정보 조회