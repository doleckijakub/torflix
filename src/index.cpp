#include <crow/http_response.h>
#include <crow/mustache.h>

namespace route {

crow::response index() {
	static auto page = crow::mustache::load("html/index.html")
		.render();
	return crow::response(page);
}

}
