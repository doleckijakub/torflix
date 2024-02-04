#include <crow/http_response.h>

#include "httpget.hpp"

#include <iomanip>
#include <sstream>
#include <cctype>

std::string url_param_encode(const std::string& input) {
	std::ostringstream encoded;
	encoded.fill('0');
	encoded << std::hex;

	for (char c : input) {
		if (std::isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
			encoded << c;
		} else if (c == ' ') {
			encoded << '+';
		} else {
			encoded << '%' << std::setw(2) << (int) (unsigned char) c;
		}
	}

	return encoded.str();
}

namespace route {

crow::response search(const char *query) {
	using namespace std::string_literals;
	auto apibay_json = httpget("https://apibay.org/q.php?q="s + url_param_encode(query) + "&cat=0"s);
	return crow::response(apibay_json);
}

}
