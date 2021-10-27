#include <App.h>

#include "sqlwrapper.h"
#include "helpers.h"

#define BOOTSTRAP_CSS "3rd_party/bootstrap-5.1.3/dist/css/bootstrap.min.css"
#define CUSTOM_CSS "misc/html/grid.css"
#define HTML_INDEX "misc/html/index.html"
#define PORT 3000

struct us_listen_socket_t *globalListenSocket;


int main(int argc, char **argv) {
	sqlwrapper *SQL = new sqlwrapper;
	SQL->InitTables();
	if (argc < 2) {
		std::cout << "provide password in argv... \n closing program...\n";
		return -1;
	}

	uWS::SSLApp app = uWS::SSLApp({
	  .key_file_name = "misc/key.pem",
	  .cert_file_name = "misc/cert.pem",
	  .passphrase = argv[1]
	}).get("/*", [](auto *res, auto *req) {
		res->end(return_file(HTML_INDEX));
	}).get("/bootstrap.min.css", [](auto *res, auto *req) {
	    res->end(return_file(BOOTSTRAP_CSS));
	}).get("/grid.css", [](auto *res, auto *req) {
	    res->end(return_file(CUSTOM_CSS));
	}).post("/adduser", [SQL](auto *res, auto *req) {
		res->onData([res, req, SQL](std::string_view data, bool last) mutable {
			std::string buffer;
			urldecode(buffer, data.data());
		
			User u = {.email = std::string(get_token(buffer, "mail=")),
					  .name = std::string(get_token(buffer, "name=")), 
					  .hashpass = std::string(get_token(buffer, "pass=")), 
					  .books = std::string("")};
			if (u.validate())
				SQL->Add(u);
			res->end("thanks for new user");
		});
		res->onAborted([](){});
	}).post("/addbook", [SQL](auto *res, auto *req) {
		res->onData([res, req, SQL](std::string_view data, bool last) mutable {
			std::string buffer;
			urldecode(buffer, data.data());
		
			Book b = {.id = std::string(get_token(buffer, "id")),
					  .title = std::string(get_token(buffer, "title")),
					  .author = std::string(get_token(buffer, "author")),
					  .category = std::string(get_token(buffer, "category")),
					  .rating = std::string(get_token(buffer, "rating")),
 					  .rentper = std::string(get_token(buffer, "rentper")),
					  .datecomp = std::string(get_token(buffer, "datecomp")),
					  .pages = std::string(get_token(buffer, "pages")),
					  .read = std::string(get_token(buffer, "read")),
					  .image = std::string(get_token(buffer, "image")),
 					  .fav = std::string(get_token(buffer, "fav")),
					  .desc = std::string(get_token(buffer, "desc")),
					  .shelves = std::string(get_token(buffer, "shelves")),
					  .tags = std::string(get_token(buffer, "tags"))};
			if (b.validate()) {
				if (SQL->Add(b)) {
					
				}
			}
			res->end("thanks for new book");
		});
		res->onAborted([](){});
	}).listen(PORT, [](auto *listen_socket) {
	    if (listen_socket) {
			std::cout << "Listening on port " << PORT << std::endl;
			globalListenSocket = listen_socket;
	    }
	});


	app.run();

	std::cout << "Failed... \n";
}

/* TODO: 
		add logger class
*/