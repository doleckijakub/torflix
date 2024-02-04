#include <crow/http_response.h>

#include <rapidjson/document.h>

#include "httpget.hpp"

#include <iomanip>
#include <sstream>
#include <cctype>
#include <cassert>

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

#define rj_str(e, k) e.HasMember(k) ? (assert(e[k].IsString()), e[k].GetString()) : ""

namespace route {

crow::response search(const char *query) {
	using namespace std::string_literals;
	
	auto apibay_resp = httpget(
		"https://apibay.org/q.php?q="s +
		url_param_encode(query) +
		"&cat=0"s
	);

	std::string resp;

	rapidjson::Document doc;
	doc.Parse(apibay_resp.c_str());
	assert(doc.IsArray());
	
	for(auto &entry : doc.GetArray()) {
		assert(entry.IsObject());
		std::string name = rj_str(entry, "name");
		std::string info_hash = rj_str(entry, "info_hash");
		
		resp += "<a href=\"/stream/"s + info_hash + "\">" + name + "</a><br>";
	}

	return crow::response(resp);
}

}
