#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sqlite3.h"


/**************** DB ���� �⺻ ****************/
// �ݹ� �Լ� - SELECT ���� ����� ó���ϴ� �Լ�
static int callback(void* NotUsed, int argc, char** argv, char** azColName) {
	int i;
	for (i = 0; i < argc; i++) {
		printf("%s: %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}
static int callback_login(void* user_count_ptr, int argc, char** argv, char** azColName) {
	int* user_count = (int*)user_count_ptr;
	*user_count = atoi(argv[0]); // ù ��° �� ���� ������ ��ȯ�Ͽ� user_count�� ����
	printf("Callback called: user count = %d\n", *user_count);
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
		"BALANCE INTEGER NOT NULL DEFAULT 0,"\
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
		"COMPANY_NAME TEXT UNIQUE NOT NULL," \
		"CURRENT_PRICE INTEGER NOT NULL," \
		"COST_PRICE INTEGER NOT NULL," \
		"STOCK_COUNT INTEGER NOT NULL);";
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

/**************** ȸ�� ���� DB �Լ� ****************/
// 2.1 ȸ�� ���� �߰� �Լ�
int insert_member(sqlite3* db, const char* name, const char* user_id, const char* password) {
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
int delete_member(sqlite3* db, const char* user_id, const char* password) {
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
int member_login(sqlite3* db, const char* user_id, const char* password) {
	// ��ȿ�� ��������� Ȯ��
	if (!is_valid_user(db, user_id, password)) {
		fprintf(stderr, "Invalid user credentials\n");
		return "NONE"; // ��ȿ���� ���� ������� ��� NULL ��ȯ
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
	printf("%d\n", user_count);
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
	int rc = sqlite3_exec(db, sql_check_duplicate_key, callback, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return -1; // ���� �߻� �� -1 ��ȯ
	}
	return key_count; // �ߺ��� Ű�� ������ 1 �̻��� �� ��ȯ, ������ 0 ��ȯ
}

// 2.4 ȸ�� �α׾ƿ� �Լ�
int member_logout(sqlite3* db, const char* session) {
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
// �ż� �ֹ� ����
int insert_buyOrder(sqlite3* db, const int* stock_id, const int* stock_count) {
	char* zErrMsg = 0;
	char sql_insert_buyOrder[500];
	sprintf(sql_insert_buyOrder, "UPDATE STOCK SET CURRENT_PRICE = CURRENT_PRICE + 1000, STOCK_COUNT = STOCK_COUNT - %d WHERE ID = %d;",
		*stock_count, *stock_id);
	int rc = sqlite3_exec(db, sql_insert_buyOrder, callback, 0, &zErrMsg);
	
	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return 0;
	}
	else {
		fprintf(stdout, "Buy Order executed Successfully\n");
		return 1;
	}
}

// �ŵ� �ֹ� ����
insert_sellOrder(sqlite3* db, const int* stock_id, const int* stock_count) {
	char* zErrMsg = 0;
	char sql_insert_sellOrder[500];
	sprintf(sql_insert_sellOrder, "UPDATE STOCK SET CURRENT_PRICE = CURRENT_PRICE - 1000, STOCK_COUNT = STOCK_COUNT + %d WHERE ID = %d",
		*stock_count, *stock_id);
	int rc = sqlite3_exec(db, sql_insert_sellOrder, callback, 0, &zErrMsg);

	if (rc != SQLITE_OK) {
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
		return 0;
	}
	else {
		fprintf(stdout, "Sell Order executed Successfully\n");
		return 1;
	}
}