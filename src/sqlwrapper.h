#include "sqlite3.h"

#include <string>

#define DBNAME "books.db"
const std::string BOOKTABLE = "BookTable";
const std::string USERTABLE = "UserTable";

struct Book {
    std::string id;
    std::string title;
    std::string author;
    std::string category;
    std::string rating;
    std::string rentper;
    std::string datecomp;
    std::string pages;
    std::string read;
    std::string image;
    std::string fav;
    std::string desc;
    std::string shelves;
    std::string tags;
};

struct User {
    std::string email;
    std::string name;
    std::string hashpass;
    std::string books;

    int validate() {
        int valid=0;
        /* email */
        auto b=email.begin(), e=email.end();
        if ((b=std::find(b, e, '@')) != e && std::find(b, e, '.') != e )
            valid = 1;

        /* name */
        /* pass */        
        return valid;
    }
};

class sqlwrapper
{
public: 
	sqlwrapper();
	~sqlwrapper();

	int Add(Book b);
	int Add(User u);
	int Delete(Book b);
	int Delete(User u);
	int InitTables();
    int StrFromSql(const char *sql, std::string &result, int col);
    int LinkBookToUser(std::string userid, std::string bookid);
	int Edit();
	int GetUserBooks(std::string userid);
    std::string GetAllUsers();

private:
    int SqlExec(const char* sql);
	const char *dbName_ = DBNAME;
	sqlite3 *db_;
};
