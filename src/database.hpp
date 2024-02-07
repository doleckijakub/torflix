#pragma once

#include <sqlite3pp.h>
#include <string>

struct TorrentInfo {
	std::string name;
	std::string info_hash;
};

struct TitleInfo {
	std::string title;
	std::string year;
	std::string runtime;
	std::string plot;
	std::string img_src;
};

class Database {
public:

	static bool has_title(const std::string &imdb_id);
	static TitleInfo get_title(const std::string &imdb_id);
	static void add_title(const std::string &imdb_id, const TitleInfo &title_info);

private:

	sqlite3pp::database db;

	Database(const char *db_name) : db(db_name) {}

	static Database &get_instance();
};
