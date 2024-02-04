#include <crow/http_response.h>

#include "httpget.hpp"

namespace route {

crow::response search(const char *query) {
	using namespace std::string_literals;
	auto apibay_json = httpget("https://apibay.org/q.php?q="s + query + "&cat=0"s);
	return crow::response(apibay_json);
}

}
