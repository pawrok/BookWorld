#include <iostream>
#include <boost/format.hpp>

#include "sqlwrapper.h"

int LogSql(void *a_param, int argc, char **argv, char **column) {
    for (int i=0; i< argc; i++)
        printf("%s,\t", argv[i]);
    printf("\n");
    return 0;
}

sqlwrapper::sqlwrapper() {
    int rc = sqlite3_open(dbName_, &db_);
}

sqlwrapper::~sqlwrapper() {
    int rc = sqlite3_close(db_);
}

int sqlwrapper::SqlExec(const char* sql) {
    char *zErrMsg = 0;
    int rc = sqlite3_exec(db_, sql, LogSql, 0, &zErrMsg);
	if( rc!=SQLITE_OK ) {
	    sqlite3_free(zErrMsg);
        printf("error: %s", sqlite3_errmsg(db_));
	}
    std::cout << "SqlExec rc: " << rc << "\n";
    return rc;
}

int sqlwrapper::StrFromSql(const char *sql, std::string &result, int col = 0) {
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(db_, sql, -1, &stmt, nullptr);
    if (rc == SQLITE_OK) {
        while ((rc = sqlite3_step(stmt) == SQLITE_ROW)) {
            result.append(str(boost::format("id: %s, name: %s\n") 
                                            % sqlite3_column_text(stmt, 0)
                                            % sqlite3_column_text(stmt, 1)));
        }
        if (rc == SQLITE_DONE) {
            return sqlite3_finalize(stmt);
        } else {
            printf("error: ", sqlite3_errmsg(db_));
            return 0;
        }
    } else {
        std::cout << "StrFromSql fail!\n";
        return 0;
    }
}

int sqlwrapper::InitTables() {
    const char *sql =
        " CREATE TABLE if not exists BookTable ( " 
        "    id INTEGER PRIMARY KEY, "
        "    title TEXT, "
        "    author TEXT, "
        "    category TEXT, "
        "    rating INTEGER, "
        "    rentedper TEXT, "
        "    datecomp TEXT, "
        "    pages INTEGER, "
        "    read INTEGER, "
        "    image TEXT, "
        "    fav INTEGER, "
        "    desc TEXT, "
        "    shelves TEXT, "
        "    tags TEXT "
        " );"
        " CREATE TABLE if not exists UserTable ( " 
        "    email TEXT PRIMARY KEY, "
        "    name TEXT, "
        "    hashpass TEXT, "
        "    books TEXT "
        " );";
    return SqlExec(sql);
}

int sqlwrapper::Add(Book b) {
    std::string values = str(boost::format(
        "%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s")
        % b.id % b.title % b.author % b.category % b.rating 
        % b.rentper % b.datecomp % b.pages % b.read % b.image 
        % b.fav % b.desc % b.shelves % b.tags);
    
    std::string sql = str(boost::format(
        "INSERT INTO BookTable VALUES (%s);") % values);
    std::cout << sql << "\n";
    return SqlExec(sql.c_str());
}

int sqlwrapper::Add(User u) {
    std::string sql = str(boost::format(
        "INSERT INTO UserTable VALUES (\'%s\', \'%s\', \'%s\', \'%s\');")
        % u.email % u.name % u.hashpass % u.books);
    std::cout << sql << "\n";
    return SqlExec(sql.c_str());
}

int sqlwrapper::Delete(Book b) {
    std::string sql = str(boost::format(
        "DELETE FROM BookTable where id = %s;") % b.id);
    return SqlExec(sql.c_str());
}

int sqlwrapper::Delete(User u) {
    std::string sql = str(boost::format(
        "DELETE FROM UserTable where email = %s;") % u.email);
    return SqlExec(sql.c_str());
}

int sqlwrapper::LinkBookToUser(std::string userid, std::string bookid) {
    std::string sql = str(boost::format(
        "SELECT books FROM UserTable where id = %s;") % userid);
    std::string books;
    StrFromSql(sql.c_str(), books);
    books.append(bookid + ",");
    std::string sql2 = str(boost::format(
        "INSERT INTO UserTable(books) VALUES (%s) \
         ON CONFLICT(books) DO UPDATE SET books=excluded.books;") % books);

    return SqlExec(sql2.c_str());
}

std::string sqlwrapper::GetAllUsers() {
    std::string sql = "SELECT * FROM UserTable;";
    std::string result;
    StrFromSql(sql.c_str(), result);
    std::cout << "All users: \n" << result << "\n";
    return result;
}

// TODO: 
//      adding books to user
//      getting user's books with details
//      fix ids in tables (they're not needed)
//      change StrFromSql() to return str
//      add logging system
