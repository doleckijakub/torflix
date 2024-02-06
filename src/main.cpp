#include <crow.h>

#include <string>
#include <cassert>
#include <filesystem>

#if defined(_WIN32)
#	include <Windows.h>
#	include <ShlObj_core.h>
#elif defined(__linux__) || defined(__APPLE__)
#	include <unistd.h>
#endif

static std::string get_exec_path() {
#if defined(_WIN32)
	char buffer[MAX_PATH];
	GetModuleFileNameA(nullptr, buffer, MAX_PATH);
	return buffer;
#elif defined(__linux__) || defined(__APPLE__)
	char buffer[PATH_MAX];
	ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
	assert(len != -1);
	buffer[len] = '\0';
	return buffer;
#else
#	error "Unknown OS"
#endif
}

static std::string get_base_path() {
	return std::filesystem::path(get_exec_path())
		.parent_path()
		.parent_path()
		.string();
}

namespace route {
	crow::response index();
	crow::response search(const char *query);
	crow::response stream(const std::string &info_hash);
}

class Logger : public crow::ILogHandler {
	const char *to_str(crow::LogLevel level) {
		switch (level) {
			case crow::LogLevel::Debug: return "Debug";
        	case crow::LogLevel::Info: return "Info";
        	case crow::LogLevel::Warning: return "Warning";
        	case crow::LogLevel::Error: return "Error";
        	case crow::LogLevel::Critical: return "Critical";
		}

		assert(0 && "unreachable");
	}

public:

	Logger() {}

	void log(std::string message, crow::LogLevel level) {
		std::cerr << "[" << to_str(level) << "] " << message << std::endl;
	}
};

int main() {
	Logger logger;
	crow::logger::setHandler(&logger);

	crow::SimpleApp app;

	std::string base_path = get_base_path();
	CROW_LOG_INFO << "Base path: " << base_path;
	crow::mustache::set_global_base(base_path);

	CROW_ROUTE(app, "/")
		.methods("GET"_method)
		([]() {
			return route::index();
		});

	CROW_ROUTE(app, "/search")
		.methods("GET"_method)
		([](const crow::request &req) {
			auto query = req.url_params.get("q");
			if (query) {
				return route::search(query);
			} else {
				crow::response res;
				res.redirect("/");
				return res;
			}
		});

	CROW_ROUTE(app, "/stream/<string>")
		.methods("GET"_method)
		([](const std::string &info_hash) {
			return route::stream(info_hash);
		});
	
	app.port(8080).multithreaded().run();
}
