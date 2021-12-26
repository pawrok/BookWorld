#include <iostream>
#include <fmt/core.h>

#include "SQLWrapper.h"


/* Simple sql command execution without returned value */
int SQLWrapper::ExecSQL(const char* sql) {
    char *zErrMsg = 0;
    int rc = sqlite3_exec(db_, sql, NULL, 0, &zErrMsg);
	if (rc != SQLITE_OK) {
	    sqlite3_free(zErrMsg);
        log::error("ExecSQL error: {}, rc: '{}'", sqlite3_errmsg(db_), rc);
        log::debug("error while executing sql: '{}'", sql);
	}
    return rc;
}

int SQLWrapper::Validate(DataObject& /* obj */, SQLTable type) {
    switch (type)
    {
    case BOOK:
        break;
    case USER:
        break;
    case USERBOOKS:
        break;
    default:
        break;
    }
    return OK;
}

std::string SQLWrapper::GetValues(DataObject& obj) {
    std::string result;
    for (auto &&i : obj.properties) {
        result.append("\"" + i.second + "\"" + ",");
    }
    result.pop_back();
    log::debug("GetValues method, result is :'{}'", result);
    return result;
}

/* Get formated result from executing sql command */
/* columns are separeted by ';', while rows by '\n' */
std::string SQLWrapper::GetFromSQL(const char *sql) {
    std::string result;
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc == SQLITE_OK) {
        while ((rc = sqlite3_step(stmt) == SQLITE_ROW)) {
            for (int col=0; col<sqlite3_column_count(stmt); col++) {
                result.append(fmt::format("{};", sqlite3_column_text(stmt, col)));
            }
            result.append("\n");
        }
        if (rc == SQLITE_DONE) {
            sqlite3_finalize(stmt);
            return result;
        } else {
            log::error("GetFromSQL error: '{}'", sqlite3_errmsg(db_));
            return nullptr;
        }
    } else {
        log::error("GetFromSQL 'prepare_v2' error");
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
            "desc TEXT"
        " );"
        "CREATE TABLE if not exists UserTable (" 
            "email TEXT PRIMARY KEY,"
            "name TEXT,"
            "hashpass TEXT"
        " );";
    log::debug("Init BookTable and UserTable if they don't exist.");
    return ExecSQL(sql);
}

int SQLWrapper::CreateUserTable(std::string_view user_email) {
    std::string sql = fmt::format(
        "CREATE TABLE if not exists {} ("
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
    , user_email );
    return ExecSQL(sql.c_str());
}

/* Generic addition of proper data into BookTable or UserTable */
int SQLWrapper::Add(DataObject& obj, SQLTable type) {
    if (Validate(obj, type) == OK) {
        std::string values = GetValues(obj);
        std::string sql = fmt::format(
            "INSERT INTO {} VALUES ({});", table_name[type], values);

        return ExecSQL(sql.c_str());
    } else
        return NOT_VALID;
}

/* Addition of user's book into his table named with email */
int SQLWrapper::Add(DataObject& user_book, std::string &user_email) {
    if (Validate(user_book, USERBOOKS) == OK) {
        std::string values = GetValues(user_book);
        std::string sql = fmt::format(
            "INSERT INTO {} VALUES({}));", user_email, values);

        return ExecSQL(sql.c_str());
    } else
        return NOT_VALID;
}

/* Generic object deletion from table by primary key */
int SQLWrapper::Delete(DataObject& /* obj */, SQLTable type) {
    std::string sql = fmt::format(
        "DELETE FROM {} where id = {};", table_name[type], DataObject::GetPK(type));
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
    else {
        return EINVAL;
        log::error("GetLastBookID error");
    }
}

std::string SQLWrapper::GetAll(SQLTable type) {
    std::string sql = fmt::format("SELECT * FROM {};", table_name[type]);
    return GetFromSQL(sql.c_str());
}

SQLWrapper::SQLWrapper() {
    if (sqlite3_open(DBNAME, &db_) == SQLITE_OK)
        InitTables();
    else
        log::error("sqlite3 .db opened with error");
}

SQLWrapper::~SQLWrapper() {
    sqlite3_close(db_);
}

// TODO: 
//      adding books to user
//      getting user's books with details
//      auth tokens
