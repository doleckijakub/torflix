#include <crow/http_response.h>
#include <crow/mustache.h>

#include <rapidjson/document.h>

#include "httpget.hpp"

#include <iomanip>
#include <sstream>
#include <cctype>
#include <cassert>
#include <unordered_map>
#include <vector>

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

std::string assert_getenv(const char *name) {
	const char *env = getenv(name);
	
	if (env == nullptr) {
		using namespace std::string_literals;
		throw std::runtime_error("Environment variable: "s + name + " not set"s);
	}

	return env;
}

namespace route {

crow::response search(const char *query) {
	const static auto page = crow::mustache::load("html/search.html");
	const static auto search_result = crow::mustache::load("html/search-result.html");
	const static auto omdb_api_key = assert_getenv("OMDB_API_KEY");
	
	using namespace std::string_literals;

	auto apibay_resp = httpget(
		"https://apibay.org/q.php?q="s +
		url_param_encode(query) +
		"&cat=0"s
	);

	struct TorrentInfo {
		std::string name;
		std::string info_hash;
	};

	std::unordered_map<std::string, std::vector<TorrentInfo>> torrents;

	rapidjson::Document doc;
	doc.Parse(apibay_resp.c_str());
	assert(doc.IsArray());
	
	for(auto &entry : doc.GetArray()) {
		assert(entry.IsObject());
		std::string name = rj_str(entry, "name");
		std::string info_hash = rj_str(entry, "info_hash");
		std::string imdb = rj_str(entry, "imdb");

		if(imdb.length()) {
			torrents[imdb].emplace_back(TorrentInfo { name, info_hash });
		}	
	}
	
	std::string search_results;
	for(const auto &[imdb, torrent_infos] : torrents) {
		crow::mustache::context ctx;
		ctx["title"] = torrent_infos[0].name; // TODO: omdb api
		ctx["num_torrents"] = std::to_string(torrent_infos.size());
		ctx["img_src"] = ""; // TODO: obdb api
		ctx["plot"] = "plot here"; // TODO: omdb api
		search_results += search_result.render_string(ctx);
	}

	crow::mustache::context ctx;
	ctx["query"] = query;
	ctx["search_results"] = search_results;

	return crow::response(page.render(ctx));
}

}
