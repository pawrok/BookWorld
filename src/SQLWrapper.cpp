#include <iostream>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

#include "SQLWrapper.h"


SQLWrapper::SQLWrapper() {
    if (sqlite3_open(DBNAME, &db_) == SQLITE_OK)
        InitTables();
}

SQLWrapper::~SQLWrapper() {
    sqlite3_close(db_);
}

/* Simple sql command execution without returned value */
int SQLWrapper::ExecSQL(const char* sql) {
    char *zErrMsg = 0;
    int rc = sqlite3_exec(db_, sql, NULL, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
	    sqlite3_free(zErrMsg);
        printf("error: %s", sqlite3_errmsg(db_));
	}
    std::cout << "ExecSQL rc: " << rc << "\n";
    return rc;
}

/* Get formated result from executing sql command */
/* columns are separeted by ';', while rows by '\n' */
std::string &&SQLWrapper::GetFromSQL(const char *sql) {
    std::string result;
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc == SQLITE_OK) {
        while ((rc = sqlite3_step(stmt) == SQLITE_ROW)) {
            for (int col=0; col<sqlite3_column_count(stmt); col++) {
                result.append(str(boost::format("%s;")
                    % sqlite3_column_text(stmt, col)));
            }
            result.append("\n");
        }
        if (rc == SQLITE_DONE) {
            sqlite3_finalize(stmt);
            return std::move(result);
        } else {
            printf("error: ", sqlite3_errmsg(db_));
            return nullptr;
        }
    } else {
        std::cout << "StrFromSQL fail!\n";
        return nullptr;
    }
}

int SQLWrapper::InitTables() {
    const char *sql =
        "CREATE TABLE if not exists BookTable (" 
            "isbn TEXT PRIMARY KEY, "
            "title TEXT,"
            "author TEXT,"
            "category TEXT,"
            "pages TEXT,"
            "image TEXT,"
            "desc TEXT,"
        " );"
        "CREATE TABLE if not exists UserTable (" 
            "email TEXT PRIMARY KEY,"
            "name TEXT,"
            "hashpass TEXT,"
        " );";    
    return ExecSQL(sql);
}

int SQLWrapper::CreateUserTable(std::string_view user_email) {
    std::string sql = str(boost::format(
        "CREATE TABLE if not exists %s ("
            "isbn INTEGER PRIMARY KEY,"
            "rating INTEGER,"
            "rented_to TEXT,"
            "date_comp TEXT,"
            "read TEXT,"
            "fav TEXT,"
            "shelves TEXT,"
            "tags TEXT,"
            "FOREIGN KEY (isbn) REFERENCES BookTable(isbn)"
        ");"
    ) % user_email );
    return ExecSQL(sql.c_str());
}

/* Generic addition of proper data into BookTable or UserTable */
int SQLWrapper::Add(DataObject& obj, SQLTable type) {
    if (Validate(obj, type) == OK) {
        std::string values = GetValues(obj);
        std::string sql = str(boost::format(
            "INSERT INTO %s VALUES (%s);") % table_name[type] % values);

        return ExecSQL(sql.c_str());
    } else
        return NOT_VALID;
}

/* Addition of user's book into his table named with email */
int SQLWrapper::Add(DataObject& user_book, std::string &user_email) {
    if (Validate(user_book, USERBOOKS) == OK) {
        std::string values = GetValues(user_book);
        std::string sql = str(boost::format(
            "INSERT INTO %s VALUES(%s));") % user_email % values);

        return ExecSQL(sql.c_str());
    } else
        return NOT_VALID;
}

/* Generic object deletion from table by primary key */
int SQLWrapper::Delete(DataObject& obj, SQLTable type) {
    std::string sql = str(boost::format(
        "DELETE FROM %s where id = %s;") % table_name[type] % DataObject::GetPK(type));
    return ExecSQL(sql.c_str());
}

int SQLWrapper::GetLastBookID() {
    const char* sql = "SELECT MAX(isbn) FROM BookTable;";
    sqlite3_stmt *stmt;
    int last_book = 0;
    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc == SQLITE_OK) {
        while ((rc = sqlite3_step(stmt) == SQLITE_ROW)) {
            last_book = sqlite3_column_int(stmt, 0);
        }
    }
    if (rc == SQLITE_DONE)
        sqlite3_finalize(stmt);
    
    if (last_book > 0)
        return last_book;
}

std::string &&SQLWrapper::GetAll(SQLTable type) {
    std::string sql = str(boost::format("SELECT * FROM %s;") % table_name[type]);
    return GetFromSQL(sql.c_str());
}

// TODO: 
//      adding books to user
//      getting user's books with details
//      fix ids in tables (they're not needed)
//      change StrFromSQL() to return str
//      add logging system
