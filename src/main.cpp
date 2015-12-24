#include "common.h"

#include "http/client.h"
#include "http/request.h"
#include <fstream>
#include <iterator>
#include <cassert>

#include "common/log.h"
#include "common/config.h"
#include "db/connection.h"
#include "db/prepared_statement.h"

#include <cstdlib>
//#include <mysql_connection.h>
//#include <cppconn/driver.h>
//#include <cppconn/exception.h>
//#include <cppconn/resultset.h>
//#include <cppconn/statement.h>
#include <mysql.h>

using namespace http;

int main(int argc, char* argv[]) {
	std::string configFile = "C:/dev/podbot_asio/res/podbot.cfg";

	PBLOG_INFO << "  ____           _ _           _   ";
	PBLOG_INFO << " |  _ \\ ___   __| | |__   ___ | |_ ";
	PBLOG_INFO << " | |_) / _ \\ / _` | '_ \\ / _ \\| __|";
	PBLOG_INFO << " |  __/ (_) | (_| | |_) | (_) | |_ ";
	PBLOG_INFO << " |_|   \\___/ \\__,_|_.__/ \\___/ \\__|";
	PBLOG_INFO;
	PBLOG_INFO << "Config file: " << configFile;
	PBLOG_INFO << "OpenSSL version: " << OPENSSL_VERSION_TEXT;
	PBLOG_INFO << "Boost version: " << BOOST_VERSION / 100000 << "." << BOOST_VERSION / 100 % 1000 << "." << BOOST_VERSION % 100;
	PBLOG_INFO << "<Ctrl-C> to stop.";

	auto cfgMgr = std::make_unique<ConfigMgr>();
	cfgMgr->ParseCommandLine(argc, argv);
	cfgMgr->ParseConfigFile(configFile);

	mysql_library_init(0, nullptr, nullptr);
	auto conn = new db::Connection(cfgMgr.get());
	conn->Open();

	auto ps = new db::PreparedStatement("INSERT INTO one (sint, sstring, ssbool) VALUES (?,?,?)");
	ps->set_int32(0, 456);
	ps->set_string(1, "this is also a string");
	ps->set_bool(2, false);

	conn->Execute(ps);

	//try {
	//	auto request = std::make_unique<Request>();
	//	request->set_url(Url("https://danvonk.com"));

	//	http_client* cl = new http_client();
	//	//auto res = cl->Req(request.get());
	//	//std::cout << "Response size: " << res->get_content().size() << "\n";
	//}
	//catch (const std::exception& e) {
	//	std::cerr << "Exception: " << e.what() << "\n";
	//}

	//delete conn;
	//delete ps;
	return 0;
}