#include <crow.h>

namespace route {

crow::response search(const char *query) {
	return crow::response(query);
}

}
