#include <string>
#include <map>
#include <array>
#include <vector>
#include <list>
#include <sqlite3.h>

#include "helpers.h"

#define DBNAME "books.db"


/* Types of object stored in database. Each object has it's own table. */
typedef enum SQLTable {
    BOOK, USER, USERBOOKS
};

std::map<SQLTable, std::string> table_name = {
    {BOOK, "Books"},
    {USER, "Users"}
};

std::map<SQLTable, std::vector<std::string>> table_values = {
    {BOOK, {"isbn", "title", "author", "category", "image", "desc", "pages"}},
    {USER, {"email", "name", "pass"}},
    {USERBOOKS, {"isbn", "title", "author", "category", "image", "desc", "pages"}}
};

class DataObject {
public:
    DataObject(std::string& buffer, SQLTable type) {
        for (auto &&i : table_values[type])
            properties[i] = helpers::get_token(buffer, i);      
    }

    static std::string GetPK(SQLTable type) {
        return table_values[type][0];
    }

    /* name, value */
    std::map<std::string, std::string> properties;
};

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

    // User GetUser(std::string_view ver_token);

	SQLWrapper();
	~SQLWrapper();

    /* For testing purposes */
    std::string &&GetAll(SQLTable type);

private:
    int Validate(DataObject& obj, SQLTable type);
    int ExecSQL(const char* sql);
    std::string &&GetValues(DataObject& obj);
    std::string &&GetFromSQL(const char *sql);
	sqlite3 *db_;
};