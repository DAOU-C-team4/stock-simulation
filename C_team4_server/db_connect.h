#include "sqlite3.h"

extern sqlite3* db;

static int callback(void* NotUsed, int argc, char** argv, char** azColName);
sqlite3* open_database(const char* db_name);
int create_member_table(sqlite3* db);
int insert_member(sqlite3* db, const char* name, 
	int age, const char* email, const char* user_id, const char* password);