#include <App.h>

#include "SQLWrapper.h"

#define BOOTSTRAP_CSS "3rd_party/bootstrap-5.1.3/dist/css/bootstrap.min.css"
#define CUSTOM_CSS "misc/html/grid.css"
#define HTML_INDEX "misc/html/index.html"
#define PORT 3000
#define LOCAL_ADDRESS "https://172.28.0.203"

// struct us_listen_socket_t *globalListenSocket;


int main(int argc, char **argv) {
	if (argc < 2) {
		std::cout << "Provide password in argv... \n Closing program...\n";
		return -1;
	}
	spdlog::set_level(spdlog::level::debug); // Set global log level to debug

	SQLWrapper *SQL = new SQLWrapper;

	uWS::SSLApp app = uWS::SSLApp({
	  .key_file_name = "misc/key.pem",
	  .cert_file_name = "misc/cert.pem",
	  .passphrase = argv[1]

	}).get("/*", [](auto *res, auto */*req*/) {
		log::debug("Got new request.");
		res->end(ReturnFile(HTML_INDEX));

	}).get("/bootstrap.min.css", [](auto *res, auto */* req */) {
	    res->end(ReturnFile(BOOTSTRAP_CSS));

	}).get("/grid.css", [](auto *res, auto */* req */) {
	    res->end(ReturnFile(CUSTOM_CSS));

	}).post("/adduser", [SQL](auto *res, auto *req) {
		res->onData([res, req, SQL](std::string_view data, bool /* last */) mutable {
			std::string buffer;
			UrlDecode(buffer, data.data());
			DataObject user(buffer, USER);
			
			if (SQL->Add(user, USER) == SQLITE_OK) {
				SQL->CreateUserTable(user.properties["email"]);
				res->end("thanks for new user");
			} else
				res->end("couldn't add new user");
		});
		res->onAborted([](){});

	}).post("/addbook", [SQL](auto *res, auto *req) {
		res->onData([res, req, SQL](std::string_view data, bool /* last */) mutable {
			std::string buffer;
			UrlDecode(buffer, data.data());
		
			DataObject book(buffer, BOOK);
			
			DataObject userbook(buffer, USERBOOKS);

			// std::string email = 
			
			if (SQL->Add(book, BOOK) == SQLITE_OK && SQL->Add(userbook, USERBOOKS) == SQLITE_OK) {
				res->end(SQL->GetAll(BOOK));
			}
			res->end("error occured [C]");
		});
		res->onAborted([](){});

	}).post("/login", [](auto *res, auto */* req */) {
		res->end(ReturnFile(HTML_INDEX));
		res->onAborted([](){});

	}).listen(PORT, [](auto *listen_socket) {
	    if (listen_socket) {
			log::info("Listening on {}:{}", LOCAL_ADDRESS, PORT);
			// globalListenSocket = listen_socket;
	    }
	});

	app.run();

	log::warn("Failed..");
}
