#include <string>
#include <map>
#include <array>
#include <vector>
#include <list>

#include <sqlite3.h>

#include "common.h"

#define DBNAME "books.db"


class SQLWrapper {
public:
	int InitTables();
	int Add(DataObject& user_book, std::string &user_email);
    int Add(DataObject& obj, SQLTable type);

	int Delete(DataObject& obj, SQLTable type);
	// int EditUser(std::string email, User u);
    int GetLastBookID();
	int GetUserBooks(std::string_view user_email);
    int CreateUserTable(std::string_view user_email);
    int CheckUser(DataObject user);

    // User GetUser(std::string_view ver_token);

	SQLWrapper();
	~SQLWrapper();

    /* For testing purposes */
    std::string GetAll(SQLTable type);

private:
    int Validate(DataObject& obj, SQLTable type);
    int ExecSQL(const char* sql);
    std::string GetValues(DataObject& obj);
    std::string GetFromSQL(const char *sql);

	sqlite3 *db_;
};