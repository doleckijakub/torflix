#include <libtorrent/entry.hpp>
#include <libtorrent/session.hpp>
#include <libtorrent/add_torrent_params.hpp>
#include <libtorrent/torrent_handle.hpp>
#include <libtorrent/magnet_uri.hpp>

std::string handle_stream_torrent(const std::string &info_hash) {
	using namespace std::string_literals;
	std::string resp;
	std::string magnet_link = "magnet:?xt=urn:btih:"s + info_hash;
	lt::session ses;
	lt::add_torrent_params atp = lt::parse_magnet_uri(magnet_link.c_str());
	libtorrent::torrent_handle handle = ses.add_torrent(atp);
	while (!handle.has_metadata()) {
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	auto ti = handle.torrent_file();
	const auto &fs = ti->files();
	for (int i = 0; i < fs.num_files(); ++i) {
		const auto &file = fs.at(i);
		resp += file.path;
		resp += " ";
		resp += std::to_string(file.size);
		resp += "\n";
	}
	return resp;
}
