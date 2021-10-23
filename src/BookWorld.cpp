#include "App.h"
#include <openssl/ssl.h>
#include <openssl/crypto.h>
#include <fstream>
#include <sstream>
#include <chrono>
#include "sqlwrapper.h"

#define BOOTSTRAPCSS "3rd_party/bootstrap-5.1.3/dist/css/bootstrap.min.css"
#define HTMLINDEX "misc/html/index.html"
#define PORT 3000

struct us_listen_socket_t *globalListenSocket;

void urldecode(std::string& dst, const char *src) {
	int hex1, hex2;
	while (*src) {
		if ((*src == '%') && ((hex1 = src[1]) && (hex2 = src[2])) && (isxdigit(hex1) && isxdigit(hex2))) {
			hex1 -= '0';
			if (hex1 > 9)
				hex1 -= 7;
			hex2 -= '0';
			if (hex2 > 9)
				hex2 -= 7;

			dst.push_back(16*hex1+hex2);
			src+=3;
		} else {
			dst.push_back(*src++);
		}
	}
	if (dst.back() == '\r')
		dst.pop_back();
}

std::string_view get_token(std::string& s, std::string_view delimeter) {
	int pos = s.find(delimeter) + delimeter.length();
	int last = s.find("&", pos);
	last = (last != std::string::npos) ? last : s.length();
	
	return std::string_view(s.cbegin() + pos, s.cbegin() + last);
}

std::string return_file(const char* path) {
	std::ifstream t(path);
	std::stringstream buffer;
	buffer << t.rdbuf();
	return buffer.str();
}

int main(int argc, char **argv) {
	sqlwrapper *SQL = new sqlwrapper;
	SQL->InitTables();

	uWS::SSLApp app = uWS::SSLApp({
	  .key_file_name = "misc/key.pem",
	  .cert_file_name = "misc/cert.pem",
	  .passphrase = "test"
	}).get("/*", [](auto *res, auto *req) {
		res->end(return_file(HTMLINDEX));
	}).get("/bootstrap.min.css", [](auto *res, auto *req) {
	    res->end(return_file(BOOTSTRAPCSS));
	}).post("/*", [SQL](auto *res, auto *req) {
		res->onData([res, req, SQL](std::string_view data, bool last) mutable {
			std::string buffer;
			urldecode(buffer, data.data());
		
			User u = {.email = std::string(get_token(buffer, "mail=")),
					  .name = std::string(get_token(buffer, "name=")), 
					  .hashpass = std::string(get_token(buffer, "pass=")), 
					  .books = std::string("")};
			if (u.validate())
				SQL->Add(u);
		});
		res->end("thanks for data");
	}).listen(PORT, [](auto *listen_socket) {
	    if (listen_socket) {
			std::cout << "Listening on port " << PORT << std::endl;
			globalListenSocket = listen_socket;
	    }
	});


	app.run();

	std::cout << "Failed... \n";
}


// TODO: 
//		 cert password from argv
// 		 parse and validate req
//		 simple js buttons for testing