#include "sqlite3.h"

extern sqlite3* db;

/**************** DB ���� �⺻ ****************/
static int callback(void* NotUsed, int argc, char** argv, char** azColName);
static int callback_login(void* user_count_ptr, int argc, char** argv, char** azColName);
sqlite3* open_database(const char* db_name);
/**************** ���̺� ���� �Լ� ****************/
int create_member_table(sqlite3* db);
int create_stock_table(sqlite3* db);
int create_account_table(sqlite3* db);
/**************** ȸ�� ���� DB �Լ� ****************/
int insert_member(sqlite3* db, const char* name, const char* user_id, const char* password);
int delete_member(sqlite3* db, const char* user_id, const char* password);
int member_login(sqlite3* db, const char* user_id, const char* password);
int generate_random_key(char* key);
int is_duplicate_key(sqlite3* db, const char* key);
int member_logout();
/**************** �ֽ� ���� DB �Լ� ****************/
