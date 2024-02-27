#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"
#include "server_request.h"

// �ֽ� id �Ľ�
matching_data[10] = { 100, 200, 300, 400, 500, 600, 700, 800, 900, 1000 };

int idx = 0;

/**************** DB ���� �⺻ ****************/
// �ݹ� �Լ� 1 - SELECT ���� ����� ó���ϴ� �Լ�
static int callback(void* NotUsed, int argc, char** argv, char** azColName) {
	return 0;
}
// �ݹ� �Լ� 2 - �α��� ���� ����� ó���ϴ� �Լ�
static int callback_login(void* user_count_ptr, int argc, char** argv, char** azColName) {
	int* user_count = (int*)user_count_ptr;
	*user_count = atoi(argv[0]); // ù ��° �� ���� ������ ��ȯ�Ͽ� user_count�� ����
	printf("Callback called: user count = %d\n", *user_count);
	return 0;
}
// �ݹ� �Լ� 3 - �ֽ� ���� �ε�
static int callback_stock_price(void* user_price_ptr, int argc, char** argv, char** azColName) {
	int* price = (int*)user_price_ptr;
	*price = atoi(argv[0]); // ù ��° �� ���� ������ ��ȯ�Ͽ� ����
	return 0;
}
// �ݹ� �Լ� 4 - �ֽ� ���� �ε�
static int callback_stock_count(void* user_count_ptr, int argc, char** argv, char** azColName) {
	int* count = (int*)user_count_ptr;
	*count = atoi(argv[0]); // ù ��° �� ���� ������ ��ȯ�Ͽ� ����
	return 0;
}
// �ݹ� �Լ� 5 - ����� �ܾ� ��ȸ
static int callback_stock_balance(void* user_balance_ptr, int argc, char** argv, char** azColName) {
	int* balance = (int*)user_balance_ptr;
	*balance = atoi(argv[0]); // ù ��° �� ���� ������ ��ȯ�Ͽ� ����
	return 0;
}
// �ݹ� �Լ� 6 - ������̵�(ID) ��ȸ (user_id �ƴ�)
static int callback_member_id(void* member_id_ptr, int argc, char** argv, char** azColName) {
	int* member_id = (int*)member_id_ptr;
	*member_id = atoi(argv[0]); // ù ��° �� ���� ������ ��ȯ�Ͽ� ����
	return 0;
}
// �ݹ� �Լ� 7 - ȸ��� ��ȸ
static int callback_company_name(void* company_name_ptr, int argc, char** argv, char** azColName) {
	char* company_name = (char*)company_name_ptr;
	strcpy(company_name, argv[0]);
	return 0;
}
// �ݹ� �Լ� 8 - ACCOUNT ���ڵ� ��ȸ
static int callback_check_accountRecord(void* b_check_accountRecord_ptr, int argc, char** argv, char** azColName) {
	int* b_check_accountRecord = (int*)b_check_accountRecord_ptr;
	*b_check_accountRecord = atoi(argv[0]);
	printf("Callback called: ACCOUNT�� ���� ���� = %d\n", *b_check_accountRecord);
	return 0;
}
// �ݹ� �Լ� 9 - ACCOUNT ���̺� QUANTITY ��ȸ (���� ���� ����)
static int callback_account_quantity(void* quantity_ptr, int argc, char** argv, char** azColName) {
	int* quantity = (int*)quantity_ptr;
	*quantity = atoi(argv[0]);
	printf("Callback called: �ش����� ���� ���� = %d\n", *quantity);
	return 0;
}
//�ݹ��Լ� 10 - ��ü �ֽ� ��ȸ
static int callback_stockinfo(void* stockinfo_ptr, int argc, char** argv, char** azColName) {
	STOCK_RES* stockinfo = (STOCK_RES*)stockinfo_ptr;
	for (int i = 0; i < argc; i++) {
		stockinfo[idx].stock_id = atoi(argv[0]); // ù ��° �� ���� ������ ��ȯ�Ͽ� ����
		stockinfo[idx].stock_name = atoi(argv[1]);
		strcpy(stockinfo[idx].stock_company_name, argv[2]);
		stockinfo[idx].stock_price = atoi(argv[3]);
		stockinfo[idx].stock_count = atoi(argv[4]);
		//printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	idx++;
	return 0;
}


// 0. �����ͺ��̽� ���� �Լ�
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

/**************** ���̺� ���� �Լ� ****************/
// 1.1 ȸ�� ���̺� ���� �Լ�
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

// 1.2 �ֽ� ���̺� �����Լ�
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

// 1.3 �ֽ��ܰ� ���̺� �����Լ�
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
		"FOREIGN KEY(STOCK_ID) REFERENCES STOCK(ID));"; // �ܺ� Ű ����
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

int insert_account_dummyData(sqlite3* db) {
	char* zErrMsg = 0;
	const char* sql_insert_data =
		"INSERT INTO ACCOUNT (MEMBER_ID, STOCK_ID, COMPANY_NAME, QUANTITY, PURCHASE_PRICE) "
		"VALUES "
		"(5, 100, 'Daou', 100, 2200000), "
		"(5, 200, 'Kiwoom Securities', 200, 1500000);";
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

/**************** ȸ�� ���� DB �Լ� ****************/
// 2.1 ȸ�� ���� �߰� �Լ�
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

// 2.2 ȸ�� ���� ���� �Լ�
int db_delete_member(sqlite3* db, const char* user_id, const char* password) {
	// ��ȿ�� ��������� Ȯ��
	if (!is_valid_user(db, user_id, password)) {
		fprintf(stderr, "Invalid user credentials\n");
		return 0; // ��ȿ���� ���� ������� ��� 0 ��ȯ
	}
	// ��ȿ�� ������Ͻ�
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

// 2.3 ȸ�� �α��� �Լ�
int db_login(sqlite3* db, const char* user_id, const char* password) {
	// ��ȿ�� ��������� Ȯ��
	if (!is_valid_user(db, user_id, password)) {
		fprintf(stderr, "Invalid user credentials\n");
		return "NONE"; // ��ȿ���� ���� ������� ��� NONE ��ȯ
	}

	// ����Ű ���� �� ����
	char access_key[50];
	int is_duplicate = 0;
	do {
		generate_random_key(access_key);
		is_duplicate = is_duplicate_key(db, access_key);
		if (is_duplicate == -1) {
			return NULL;
		}
	} while (is_duplicate > 0);

	// ������Ű db���
	char* zErrMsg = 0;
	char sql_update_access_key[200];
	sprintf(sql_update_access_key, "UPDATE MEMBER SET ACCESS_KEY = '%s' WHERE USER_ID = '%s' AND PASSWORD = '%s';", access_key, user_id, password);
	int rc = sqlite3_exec(db, sql_update_access_key, NULL, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return NULL;
	}

	// ������Ű ����
	return access_key;
}

// 2.3.0 ����ڰ� ��ȿ���� Ȯ���ϴ� �Լ�
int is_valid_user(sqlite3* db, const char* user_id, const char* password) {
	char* zErrMsg = 0;
	char sql_query[200];
	sprintf(sql_query, "SELECT COUNT(*) FROM MEMBER WHERE USER_ID = '%s' AND PASSWORD = '%s';", user_id, password);
	int user_count = 0;
	int rc = sqlite3_exec(db, sql_query, callback_login, &user_count, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return 0; // ������ �߻��� ��� ��ȿ���� ���� ����ڷ� ó��
	}
	return user_count; // ����ڰ� �����ϸ� 1, �׷��� ������ 0 ��ȯ
}

// 2.3.1 ���� ������Ű ���� �Լ�
int generate_random_key(char* key) {
	const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	srand((unsigned int)time(NULL)); // ���� �ð��� �õ�� ���
	for (int i = 0; i < 30; i++) { // ���̰� 30�� ������ ���ڿ� ����
		int index = rand() % strlen(charset);
		key[i] = charset[index];
	}
	key[30] = '\0'; // NULL ����('\0') �߰�
	return 0;
}

// 2.3.2 ������ Ű�� �����ͺ��̽��� �ߺ��Ǵ��� �˻��ϴ� �Լ�
int is_duplicate_key(sqlite3* db, const char* key) {
	char* zErrMsg = 0;
	char sql_check_duplicate_key[200];
	sprintf(sql_check_duplicate_key, "SELECT COUNT(*) FROM MEMBER WHERE ACCESS_KEY = '%s';", key);
	int key_count = 0;
	int rc = sqlite3_exec(db, sql_check_duplicate_key, NULL, &key_count, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return -1; // ���� �߻� �� -1 ��ȯ
	}
	return key_count; // �ߺ��� Ű�� ������ 1 �̻��� �� ��ȯ, ������ 0 ��ȯ
}

// 2.4 ȸ�� �α׾ƿ� �Լ�
int db_logout(sqlite3* db, const char* session) {
	// ���⼭�� �ܼ��� �α׾ƿ� �޽����� ����ϴ� ������ �����մϴ�.
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

/**************** �ֽ� ���� DB �Լ� ****************/
// (�۾��ؾ���!!!!!!!!) 3.0 �ֽ� ������ȸ
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
		return stockinfo; // ������ �߻��� ��� ��ȿ���� ���� ����ڷ� ó��
	}
	/*for (int i = 0; i < 10; i++) {
		printf("stock_id[%d] = %d\n", i, stockinfo[i].stock_id);
		printf("stock_name[%d] = %d\n", i, stockinfo[i].stock_name);
		printf("stock_company_name[%d] = %s\n", i, stockinfo[i].stock_company_name);
		printf("stock_price[%d] = %d\n", i, stockinfo[i].stock_price);
		printf("stock_count[%d] = %d\n", i, stockinfo[i].stock_count);
	}*/
	return stockinfo; // ����ü ��ȯ
}

// 3.1 �ֽ� �ż�
int db_buyStock(sqlite3* db, char* session, int s_id, int s_cnt) {
	char* zErrMsg = 0;
	char sql_buyStock[500], company_name[50];
	int member_id;
	int stock_price = 0, stock_count = 0, balance = 0;
	int b_check_accountRecord = 1;
	printf("%d �ֽ� %d�� �ż�", matching_data[s_id - 1], s_cnt);

	// �ֽ� ���� ��ȸ
	char sql_query[200];
	sprintf(sql_query, "SELECT CURRENT_PRICE FROM STOCK WHERE MATCH_ID = '%d';", matching_data[s_id - 1]);
	int rc = sqlite3_exec(db, sql_query, callback_stock_price, &stock_price, &zErrMsg);
	if (rc != SQLITE_OK) {
		sqlite3_free(zErrMsg);
		return 1;
	}
	// �ֽ� �ܷ� ��ȸ
	sprintf(sql_query, "SELECT STOCK_COUNT FROM STOCK WHERE MATCH_ID = '%d';", matching_data[s_id - 1]);
	rc = sqlite3_exec(db, sql_query, callback_stock_count, &stock_count, &zErrMsg);
	if (rc != SQLITE_OK) {
		sqlite3_free(zErrMsg);
		return 1;
	}
	// �� ������ ��ȸ
	sprintf(sql_query, "SELECT BALANCE FROM MEMBER WHERE ACCESS_KEY = '%s';", session);
	rc = sqlite3_exec(db, sql_query, callback_stock_balance, &balance, &zErrMsg);
	if (rc != SQLITE_OK) {
		sqlite3_free(zErrMsg);
		return 1;
	}
	printf("�޾ƿ� �ֽ� ���� : %d, �������� : %d, �ܰ� : %d\n", stock_price, stock_count, balance);

	// ���� ���� ���̵� ��ȸ( Auto Increment ) => user_id �ƴ�
	sprintf(sql_query, "SELECT ID FROM MEMBER WHERE ACCESS_KEY = '%s';", session);
	rc = sqlite3_exec(db, sql_query, callback_member_id, &member_id, &zErrMsg);
	if (rc != SQLITE_OK) {
		sqlite3_free(zErrMsg);
		return 1;
	}
	// ȸ���̸� ��ȸ
	sprintf(sql_query, "SELECT COMPANY_NAME FROM STOCK WHERE MATCH_ID = %d;", matching_data[s_id - 1]);
	rc = sqlite3_exec(db, sql_query, callback_company_name, company_name, &zErrMsg);
	if (rc != SQLITE_OK) {
		sqlite3_free(zErrMsg);
		return 1;
	}
	// ( ���� ���� ���̵�, STOCK_ID ) �� ������ account table �� UNIQUE ���� Ȯ��
	printf("SQL���� ���� ������\n");
	printf("\n��ȸ�� ���� ���� ���̵� : %d\n", member_id);
	printf("\n��ȸ�� ȸ��� : %d\n", matching_data[s_id - 1]);
	sprintf(sql_query, "SELECT COUNT(*) FROM ACCOUNT WHERE MEMBER_ID = %d AND STOCK_ID = %d;", member_id, matching_data[s_id - 1]);
	rc = sqlite3_exec(db, sql_query, callback_check_accountRecord, &b_check_accountRecord, &zErrMsg);
	printf("SQL ���� ��� : b_check_accountRecord : %d\n\n\n", b_check_accountRecord);
	if (rc != SQLITE_OK) {
		printf("Account Record ��ȸ ���� �߻� :!!!!!\n\n\n");
		sqlite3_free(zErrMsg);
		return 1;
	}

	// ���� ���ڸ���
	if (stock_price * s_cnt > balance) {
		fprintf(stderr, "�ܾ��� �����մϴ�.\n");
		return 2;
	}
	// �ż���û �ֽ� ������ �ܷ��� �ʰ��� ��
	if (s_cnt > stock_count) {
		fprintf(stderr, "�ֽ� ������ �����մϴ�.\n");
		return 3;
	}

	// ȸ�� ���̺� - ȸ�� ��ġ�� ����
	sprintf(sql_buyStock, "UPDATE MEMBER SET BALANCE = BALANCE - %d WHERE ACCESS_KEY = '%s';", stock_price * (int)s_cnt, session);
	//printf("|||||�ż� �ݾ� �߰� : %d\n\n", stock_price * s_cnt);
	rc = sqlite3_exec(db, sql_buyStock, callback, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		sqlite3_free(zErrMsg);
		return 1;
	}
	fprintf(stdout, "Update User Money successfully\n");

	// �ֽ� ���̺� - �ֽ� ���� ����
	// �ֽ� ����, �ܷ� ����
	sprintf(sql_buyStock, "UPDATE STOCK SET STOCK_COUNT = STOCK_COUNT - %d, CURRENT_PRICE = CURRENT_PRICE + %d WHERE MATCH_ID = %d;", (int)s_cnt, calTikSize(stock_price), matching_data[s_id - 1]);
	rc = sqlite3_exec(db, sql_buyStock, callback, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		sqlite3_free(zErrMsg);
		return 1;
	}
	fprintf(stdout, "Buy Stock successfully\n");

	// ���� ���̺� - (���ID, �ֽ�ID)�� ���ڵ� ����
	if (b_check_accountRecord == 0) {
		// �ż� INSERT : ���� ���� * ���簡
		db_insert_account(db, member_id, matching_data[s_id - 1], company_name, s_cnt, stock_price); // ������ �μ� trade_category => (�ż� : 1, �ŵ� : -1)
	}
	else {
		// �ż� UPDATE : PURCHASE_PRICE = PURCHASE_PRICE + (���� ���� * ���簡)
		db_update_account(db, member_id, matching_data[s_id - 1], company_name, s_cnt, stock_price, 1);
	}

	return 0;
}

// 3.2 �ֽ� �ŵ�
int db_sellStock(sqlite3* db, char* session, int s_id, int s_cnt) {
	char* zErrMsg = 0;
	char sql_sellStock[500];
	int member_id;
	int stock_price = 0, stock_count = 0, balance = 0;
	int account_quantity = 0; // �ش� ������ ���� �ܷ�
	int b_check_accountRecord = 1; // account table �� ������ �ִ��� Ȯ��

	printf("%d �ֽ� %d�� �ŵ�", matching_data[s_id - 1], s_cnt);

	// �ֽ� ���� �� �ܷ� ��ȸ
	char sql_query[200];
	sprintf(sql_query, "SELECT CURRENT_PRICE FROM STOCK WHERE MATCH_ID = '%d';", matching_data[s_id - 1]);
	int rc = sqlite3_exec(db, sql_query, callback_stock_price, &stock_price, &zErrMsg);
	if (rc != SQLITE_OK) {
		sqlite3_free(zErrMsg);
		return 1;
	}
	// ���� ���� ���̵� ��ȸ( Auto Increment ) => user_id �ƴ�
	sprintf(sql_query, "SELECT ID FROM MEMBER WHERE ACCESS_KEY = '%s';", session);
	rc = sqlite3_exec(db, sql_query, callback_member_id, &member_id, &zErrMsg);
	if (rc != SQLITE_OK) {
		sqlite3_free(zErrMsg);
		return 1;
	}
	printf("\n\n���� ���� ���̵� ��� : %d, �����ȣ \n\n", member_id, matching_data[s_id - 1]);

	// (�۾��ؾ���!!!!!!!!) ȸ���� ���� �ֽİ��� �ҷ�����
	sprintf(sql_query, "SELECT QUANTITY FROM ACCOUNT WHERE MEMBER_ID = %d AND STOCK_ID = %d;", member_id, matching_data[s_id - 1]);
	rc = sqlite3_exec(db, sql_query, callback_account_quantity, &account_quantity, &zErrMsg);

	if (rc != SQLITE_OK) {
		sqlite3_free(zErrMsg);
		printf("quantity sql �����ٰ� ������\n\n");
		return 1;
	}

	printf("�޾ƿ� �ֽ� ���� : %d, �������� : %d\n", stock_price, account_quantity);


	// (�۾��ؾ���!!!!!!!!) ȸ���� ���� �ֽİ����� ��
	if (s_cnt > account_quantity) {
		fprintf(stderr, "���� ���� �ʰ�");
		return 2;
	}

	// ȸ�� ���̺� - ȸ�� ��ġ�� ����
	sprintf(sql_sellStock, "UPDATE MEMBER SET BALANCE = BALANCE + %d WHERE ACCESS_KEY = '%s';", stock_price * s_cnt, session);
	printf("|||||�ŵ� �ݾ� �߰� : %d\n\n", stock_price * s_cnt);
	rc = sqlite3_exec(db, sql_sellStock, callback, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		sqlite3_free(zErrMsg);
		return 1;
	}
	fprintf(stdout, "Update User Money successfully\n");
	// �ֽ� ���̺� - �ֽ� ���� ����
	// �ֽ� ����, �ܷ� ����
	sprintf(sql_sellStock, "UPDATE STOCK SET STOCK_COUNT = STOCK_COUNT + %d, CURRENT_PRICE = CURRENT_PRICE - %d WHERE MATCH_ID = '%d';", (int)s_cnt, calTikSize(stock_price), matching_data[s_id - 1]);
	rc = sqlite3_exec(db, sql_sellStock, callback, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		sqlite3_free(zErrMsg);
		return 1;
	}

	/**************************ACCOUNT TABLE CRUD***************************/
	// ���� ���̺� - (���ID, �ֽ�ID)�� ���ڵ� ����
	// �ŵ��� insert ���ʿ�
	// ���� �ܷ� == �ŵ� �ܷ� ��, ���ڵ� ����
	if (account_quantity == s_cnt) {
		db_delete_account(db, member_id, matching_data[s_id - 1]);
	}
	else {
		// �ŵ� UPDATE : PURCHASE_PRICE = PURCHASE_PRICE + (���� ���� * ���簡)
		db_update_account(db, member_id, matching_data[s_id - 1], "any data", s_cnt, stock_price, -1); // �ż� : 1, �ŵ� : -1
	}
	fprintf(stdout, "Sell Stock successfully\n");
	return 0;
}

/****************************ACCOUNT ���� DB �Լ�****************************/
// �ż� �� ACCOUNT ���̺� insert (�ŵ��� insert �Լ� ���ʿ�)
int db_insert_account(sqlite3* db, const int member_id, const int stock_id, const char* company_name, const int s_cnt, const int stock_price) {
	char* zErrMsg = 0;
	char sql_insert_account[500];
	sprintf(sql_insert_account, "INSERT INTO ACCOUNT (MEMBER_ID, STOCK_ID, COMPANY_NAME, QUANTITY, PURCHASE_PRICE) VALUES (%d, %d, '%s', %d, %d);",
		member_id, stock_id, company_name, s_cnt, stock_price);
	int rc = sqlite3_exec(db, sql_insert_account, callback, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return 0;
	}
	else {
		fprintf(stdout, "(member_id, stock_id) inserted successfully\n");
		return 1;
	}
}

int db_delete_account(sqlite3* db, const int member_id, const int stock_id) {
	char* zErrMsg = 0;
	char sql_delete_account[500];

	// �ż�, �ŵ� �ľ�
	// "UPDATE MEMBER SET ACCESS_KEY = '%s' WHERE USER_ID = '%s' AND PASSWORD = '%s';", access_key, user_id, password);
	sprintf(sql_delete_account, "DELETE FROM ACCOUNT WHERE MEMBER_ID = %d AND STOCK_ID = %d;", member_id, stock_id); // trade_category => �ż� 1, �ŵ� -1  => ��������(QUANTITY)�� �ż� �ŵ��� �°� ���
	int rc = sqlite3_exec(db, sql_delete_account, callback, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return 0;
	}
	else {
		fprintf(stdout, "(member_id, stock_id) deleted successfully\n");
		return 1;
	}
}

int db_update_account(sqlite3* db, const int member_id, const int stock_id, const char* company_name, const int s_cnt, const int stock_price, const int trade_category) {
	char* zErrMsg = 0;
	char sql_insert_account[500];

	// �ż�, �ŵ� �ľ�
	// "UPDATE MEMBER SET ACCESS_KEY = '%s' WHERE USER_ID = '%s' AND PASSWORD = '%s';", access_key, user_id, password);
	sprintf(sql_insert_account, "UPDATE ACCOUNT SET QUANTITY = QUANTITY + %d WHERE MEMBER_ID = %d AND STOCK_ID = %d;", trade_category * s_cnt, member_id, stock_id); // trade_category => �ż� 1, �ŵ� -1  => ��������(QUANTITY)�� �ż� �ŵ��� �°� ���
	int rc = sqlite3_exec(db, sql_insert_account, callback, 0, &zErrMsg);
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

// �ֽİ��� ��ƿ��Ƽ�Լ�
// ȣ������ ��� (�ְ��� ���� tikSize ȣ������ å��)
int calTikSize(int stock_price) {
	int tikSize = 1;

	if (stock_price < 2000) {
		tikSize = 1;
	}
	else if (stock_price < 5000) {
		tikSize = 5;
	}
	else if (stock_price < 20000) {
		tikSize = 10;
	}
	else if (stock_price < 50000) {
		tikSize = 50;
	}
	else if (stock_price < 200000) {
		tikSize = 100;
	}
	else if (stock_price < 500000) {
		tikSize = 500;
	}
	else {
		tikSize = 1000;
	}

	return tikSize;
}

// (�۾��ؾ���!!!!!!!!) 3.3 �� ���� ��ȸ