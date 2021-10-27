#include <sqlite3.h>

#include <string>

#define DBNAME "books.db"
const std::string BOOKTABLE = "BookTable";
const std::string USERTABLE = "UserTable";

struct Book {
    std::string id, title, author, category, rating,
                rentper, datecomp, pages, read, image,
                fav, desc, shelves, tags;

    int validate() {
        // TODO: more validation?
        return 1;
    }
};

struct User {
    std::string email, name, hashpass, books;

    int validate() {
        // TODO: more validation?
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

	int InitTables();
	int Add(Book b);
	int Add(User u);
	int Delete(Book b);
	int Delete(User u);
    int StrFromSql(const char *sql, std::string &result, int col);
    int LinkBookToUser(std::string userid, std::string bookid);
	int EditUser(std::string email, User u);
	int GetUserBooks(std::string userid);

    /* For testing purposes */
    std::string GetAllUsers();
    std::string GetAllBooks();

private:
    int SqlExec(const char* sql);
	const char *dbName_ = DBNAME;
	sqlite3 *db_;
};
