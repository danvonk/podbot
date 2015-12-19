#include "http/client.h"
#include <memory>
#include "http/request.h"

using namespace http;

int main() {
	try {
		auto request = std::make_unique<Request>();
		request->set_url(Url("http://www.43gregreg.com/"));

		http_client* cl = new http_client();
		auto res = cl->Req(request.get());
		std::cout << "Response\n" << res->get_content();
	}
	catch (const std::exception& e) {
		std::cerr << "Exception: " << e.what() << "\n";
	}
	return 0;
}