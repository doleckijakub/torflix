#include <crow/http_response.h>
#include <crow/mustache.h>

std::string handle_stream_torrent(const std::string &info_hash);

namespace route {

crow::response stream(const std::string &info_hash) {
	auto page = crow::mustache::load("html/stream.html");
	crow::mustache::context ctx;
	ctx["info_hash"] = info_hash;
	return page.render(ctx);
}

}
