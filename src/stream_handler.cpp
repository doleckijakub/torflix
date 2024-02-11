#include <functional>
#include <string>

#include <crow/logging.h>

#include <libtorrent/entry.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/magnet_uri.hpp>

namespace stream_handler {
	
void onopen(const std::string &info_hash, std::function<void(const std::string&)> send_binary) {
	using namespace std::string_literals;
	std::string magnet_link = "magnet:?xt=urn:btih:"s + info_hash;

	lt::session ses;
	lt::add_torrent_params atp = lt::parse_magnet_uri(magnet_link.c_str());
	libtorrent::torrent_handle handle = ses.add_torrent(atp);

	while (!handle.has_metadata()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

	const auto ti = handle.torrent_file();
	const auto &fs = ti->files();

	for (int i = 0; i < fs.num_files(); ++i) {
        const auto &file = fs.at(i);
        CROW_LOG_INFO << file.path;
    }
}

void onclose(const std::string &info_hash) {

}

}
