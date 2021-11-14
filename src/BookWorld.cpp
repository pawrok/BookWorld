#include <App.h>

#include "SQLWrapper.h"
#include "helpers.h"

#define BOOTSTRAP_CSS "3rd_party/bootstrap-5.1.3/dist/css/bootstrap.min.css"
#define CUSTOM_CSS "misc/html/grid.css"
#define HTML_INDEX "misc/html/index.html"
#define PORT 3000

struct us_listen_socket_t *globalListenSocket;


int main(int argc, char **argv) {
	if (argc < 2) {
		std::cout << "provide password in argv... \n closing program...\n";
		return -1;
	}
	SQLWrapper SQL;

	uWS::SSLApp app = uWS::SSLApp({
	  .key_file_name = "misc/key.pem",
	  .cert_file_name = "misc/cert.pem",
	  .passphrase = argv[1]

	}).get("/*", [](auto *res, auto *req) {
		res->end(helpers::return_file(HTML_INDEX));

	}).get("/bootstrap.min.css", [](auto *res, auto *req) {
	    res->end(helpers::return_file(BOOTSTRAP_CSS));

	}).get("/grid.css", [](auto *res, auto *req) {
	    res->end(helpers::return_file(CUSTOM_CSS));

	}).post("/adduser", [SQL](auto *res, auto *req) {
		res->onData([res, req, SQL](std::string_view data, bool last) mutable {
			std::string buffer;
			helpers::urldecode(buffer, data.data());
			DataObject user(buffer, USER);
			
			if (SQL.Add(user, USER) == SQLITE_OK) {
				SQL.CreateUserTable(user.properties["email"]);
				res->end("thanks for new user");
			} else
				res->end("couldn't add new user");
		});
		res->onAborted([](){});

	}).post("/addbook", [SQL](auto *res, auto *req) {
		res->onData([res, req, SQL](std::string_view data, bool last) mutable {
			std::string buffer;
			helpers::urldecode(buffer, data.data());
		
			DataObject book(buffer, BOOK);
			
			DataObject userbook(buffer, USERBOOKS);

			// std::string email = 
			
			if (SQL.Add(book, BOOK) == SQLITE_OK && SQL.Add(userbook, USERBOOKS) == SQLITE_OK) {
				res->end(SQL.GetAll(BOOK));
			}
			res->end("error occured [C]");
		});
		res->onAborted([](){});
	}).listen(PORT, [](auto *listen_socket) {
	    if (listen_socket) {
			std::cout << "Listening on port " << PORT << std::endl;
			globalListenSocket = listen_socket;
	    }
	});

	app.run();

	std::cout << "Failed...\n";
}
