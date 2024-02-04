#include "httpget.hpp"

#include <stdexcept>

#include <curl/curl.h>

#include <crow/logging.h>

static int writer(char *data, size_t size, size_t nmemb, std::string *writer_data) {
	if(writer_data == NULL) return 0;
	writer_data->append(data, size * nmemb);
	return size * nmemb;
}

static bool initialized_curl = false;

#define curle_ok_or_throw(func, ...) do { \
	auto code = func(conn, ##__VA_ARGS__); \
	if (code != CURLE_OK) { \
		using namespace std::string_literals; \
		throw std::runtime_error( \
				"curl error: "s + \
				std::string(#func) + \
				" ["s + \
				std::string(curl_easy_strerror(code)) + \
				"]"s \
		); \
	} \
} while(0)

std::string httpget(const std::string &url) {
	CROW_LOG_INFO << __func__ << "(" << url << ")";

	if(!initialized_curl) {
		curl_global_init(CURL_GLOBAL_DEFAULT);
		initialized_curl = true;
	}

    std::string buffer;

	CURL *conn = curl_easy_init();

    curle_ok_or_throw(curl_easy_setopt, CURLOPT_URL, url.c_str());
    curle_ok_or_throw(curl_easy_setopt, CURLOPT_FOLLOWLOCATION, 1L);
    curle_ok_or_throw(curl_easy_setopt, CURLOPT_WRITEFUNCTION, writer);
    curle_ok_or_throw(curl_easy_setopt, CURLOPT_WRITEDATA, &buffer);
	
	curle_ok_or_throw(curl_easy_perform);
	curl_easy_cleanup(conn);

	return buffer;
}
