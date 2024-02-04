#include <crow/http_response.h>

std::string handle_stream_torrent(const std::string &info_hash);

namespace route {

crow::response stream(const std::string &info_hash) {
	return handle_stream_torrent(info_hash);
}

}
