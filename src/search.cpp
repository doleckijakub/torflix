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
#include <future>

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

static std::string assert_getenv(const char *name) {
	const char *env = getenv(name);
	
	if (env == nullptr) {
		using namespace std::string_literals;
		throw std::runtime_error("Environment variable: "s + name + " not set"s);
	}

	return env;
}

struct TitleInfo {
	std::string title, year, runtime, plot, img_src;
};

static TitleInfo get_title_info(const std::string &imdb_id, const std::string &omdb_api_key) {
	using namespace std::string_literals;
	
	const auto omdb_resp = httpget(
		"https://www.omdbapi.com/?i="s +
		imdb_id + 
		"&apikey="s +
		omdb_api_key
	);

	rapidjson::Document doc;
	doc.Parse(omdb_resp.c_str());
	assert(doc.IsObject());

	return TitleInfo {
		title:   rj_str(doc, "Title"),
		year:    rj_str(doc, "Year"),
		runtime: rj_str(doc, "Runtime"),
		plot:    rj_str(doc, "Plot"),
		img_src: rj_str(doc, "Poster")
	};
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
	std::unordered_map<std::string, std::future<TitleInfo>> title_infos;

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
			if (title_infos.find(imdb) == title_infos.end()) {
				title_infos.emplace(imdb, std::async(&get_title_info, imdb, omdb_api_key));
			}
		}
	}
	
	std::string search_results;
	for(const auto &[imdb, torrent_infos] : torrents) {
		crow::mustache::context ctx;
		TitleInfo title_info = title_infos.at(imdb).get();
		ctx["title"] = title_info.title;
		ctx["num_torrents"] = std::to_string(torrent_infos.size());
		ctx["img_src"] = title_info.img_src;
		ctx["plot"] = title_info.plot;
		search_results += search_result.render_string(ctx);
	}

	crow::mustache::context ctx;
	ctx["query"] = query;
	ctx["search_results"] = search_results;

	return crow::response(page.render(ctx));
}

}
