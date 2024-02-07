#include "database.hpp"

#include <crow.h>

Database &Database::get_instance() {
	static Database instance("database.db");
	return instance;
}

bool Database::has_title(const std::string &imdb_id) {
	sqlite3pp::query query(get_instance().db, "SELECT id FROM omdb WHERE id == ?");

	query.bind(1, imdb_id, sqlite3pp::nocopy);

	for (const auto &row : query) {
		return true;
	}

	return false;
}

TitleInfo Database::get_title(const std::string &imdb_id) {
	sqlite3pp::query query(get_instance().db, "SELECT * FROM omdb WHERE id == ?");

	query.bind(1, imdb_id, sqlite3pp::nocopy);

	for (const auto &row : query) {
		return TitleInfo {
			row.get<const char *>(1),
			row.get<const char *>(2),
			row.get<const char *>(3),
			row.get<const char *>(4),
			row.get<const char *>(5),
		};
	}

	using namespace std::string_literals;
	throw std::runtime_error("Title: "s + imdb_id + " not in database"s);
}

void Database::add_title(const std::string &imdb_id, const TitleInfo &title_info) {
	sqlite3pp::command cmd(get_instance().db, "INSERT INTO omdb VALUES (?, ?, ?, ?, ?, ?)");
	cmd.bind(1, imdb_id, sqlite3pp::nocopy);
	cmd.bind(2, title_info.title, sqlite3pp::nocopy);
	cmd.bind(3, title_info.year, sqlite3pp::nocopy);
	cmd.bind(4, title_info.runtime, sqlite3pp::nocopy);
	cmd.bind(5, title_info.plot, sqlite3pp::nocopy);
	cmd.bind(6, title_info.img_src, sqlite3pp::nocopy);
	cmd.execute();
	// CROW_LOG_INFO << "SQL(" << stmt.getExpandedSQL() << ") = " << stmt.exec();
}
