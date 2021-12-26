#include <string>
#include <fstream>
#include <sstream>
#include <chrono>
#include <stdlib.h>
#include <vector>
#include <map>

#include <openssl/ssl.h>
#include <openssl/crypto.h>

#include "spdlog/spdlog.h"

#define log spdlog

#ifndef COMMON_H
#define COMMON_H

#define OK           0
#define NOT_VALID   -1

void UrlDecode(std::string& dst, const char *src);
std::string GetToken(std::string& s, std::string_view delimeter);
std::string ReturnFile(const char* path);
int ValidateIsbn(std::string& isbn);
std::string GenFakeIsbn();
std::string GenerateRandomString();


/* Types of object stored in database. */
typedef enum {
	BOOK, USER, USERBOOKS
} SQLTable;

static std::map<SQLTable, std::string> table_name = {
	{BOOK, "BookTable"},
	{USER, "UserTable"}
};

static std::map<SQLTable, std::vector<std::string>> table_values = {
	{BOOK, {"isbn", "title", "author", "category", "image", "desc", "pages"}},
	{USER, {"email=", "name=", "pass="}},
	{USERBOOKS, {"isbn", "title", "author", "category", "image", "desc", "pages"}}
};


class DataObject {
public:
	DataObject(std::string& buffer, SQLTable type) {
		for (auto &&i : table_values[type])
			properties[i] = GetToken(buffer, i);
	}

	static std::string GetPK(SQLTable type) {
		return table_values[type][0];
	}

	/* name, value */
	std::map<std::string, std::string> properties;
};

#endif