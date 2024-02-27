#include "sqlite3.h"

extern sqlite3* db;

/**************** DB ���� �⺻ ****************/
static int callback(void* NotUsed, int argc, char** argv, char** azColName);
static int callback_login(void* user_count_ptr, int argc, char** argv, char** azColName);
static int callback_stock_price(void* user_price_ptr, int argc, char** argv, char** azColName);
static int callback_stock_count(void* user_count_ptr, int argc, char** argv, char** azColName);
static int callback_stock_balance(void* user_balance_ptr, int argc, char** argv, char** azColName);
sqlite3* open_database(const char* db_name);

/**************** ���̺� ���� DB �Լ� ****************/
int create_member_table(sqlite3* db);
int create_stock_table(sqlite3* db);
int create_account_table(sqlite3* db);
int insert_stock_dummyData(sqlite3* db);
int insert_account_dummyData(sqlite3* db);
/**************** ȸ�� ���� DB �Լ� ****************/
int db_insert_member(sqlite3* db, const char* name, const char* user_id, const char* password);
int db_delete_member(sqlite3* db, const char* user_id, const char* password);
int db_login(sqlite3* db, const char* user_id, const char* password);
int generate_random_key(char* key);
int is_duplicate_key(sqlite3* db, const char* key);
int db_logout(sqlite3* db, const char* session);

/**************** �ֽ� ���� DB �Լ� ****************/
int db_allStock(sqlite3* db);
int db_buyStock(sqlite3* db, char* session, int s_id, int s_cnt);
int db_sellStock(sqlite3* db, char* session, int s_id, int s_cnt);