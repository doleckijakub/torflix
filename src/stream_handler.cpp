#include <functional>
#include <string>
#include <fstream>

#include <crow/logging.h>

#include <libtorrent/entry.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/magnet_uri.hpp>

static std::unordered_set<std::string> &open_connections() {
	static std::unordered_set<std::string> s;
	return s;
}

namespace stream_handler {
	
void onopen(const std::string &info_hash, std::function<void(const std::string&)> send_binary) {
	open_connections().emplace(info_hash);

	using namespace std::string_literals;
	std::string magnet_link = "magnet:?xt=urn:btih:"s + info_hash;

	lt::session ses;
	lt::add_torrent_params atp = lt::parse_magnet_uri(magnet_link.c_str());
	atp.save_path = "/tmp/torflix-downloads";
	libtorrent::torrent_handle handle = ses.add_torrent(atp);

	while (open_connections().find(info_hash) != open_connections().end() && !handle.has_metadata()) {
		CROW_LOG_INFO << "Awaiting metadata for " << info_hash;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

	const auto ti = handle.torrent_file();
	const auto &fs = ti->files();

	while (open_connections().find(info_hash) != open_connections().end()) {
		if (handle.status().state != libtorrent::torrent_status::seeding) {
			for (int i = 0; i < fs.num_files(); ++i) {
				const auto &file = fs.at(i);
				if (file.path.substr(file.path.find_last_of(".") + 1) == "mp4") {
					std::ifstream ifs(atp.save_path + "/"s + file.path, std::ios::binary | std::ios::ate);
					CROW_LOG_INFO << file.path << ": " << ifs.tellg();
				}
			}
		} else {
			for (int i = 0; i < fs.num_files(); ++i) {
				const auto &file = fs.at(i);
				if (file.path.substr(file.path.find_last_of(".") + 1) == "mp4") {
					std::ifstream ifs(atp.save_path + "/"s + file.path, std::ios::binary);
					std::string buffer;
					buffer.assign(std::istreambuf_iterator<char>(ifs), {});
					send_binary(buffer);
					goto sent;
				}
			}

			CROW_LOG_INFO << "No mp4 file found";

			sent:
			
			break;
		}

		std::this_thread::sleep_for(std::chrono::seconds(1));
    }
	

	CROW_LOG_INFO << "Finished streaming " << info_hash;
}

void onclose(const std::string &info_hash) {
	open_connections().erase(info_hash);
}

}
