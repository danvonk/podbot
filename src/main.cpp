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
#include "db/result.h"
#include "db/field.h"
#include <mysql.h>

#include "http/url.hpp"
#include "itunes.h"


#include <regex>

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

	//auto results = conn->ReturnExecQuery("SELECT * FROM one WHERE id=1");
	//results->NextRow();
	//std::cout << "Id is " << (*results)[1].GetU32();

	//Url url("https://danvonk.com?q=q1");
	//std::cout << url.str() << "\n";
	//std::cout << "Size of queries: " << url.query().size() << "\n";
	//std::cout << url.query(0).val() << "\n";

	Itunes it(*conn);
	it.ParseRSS("http://frogpants.com/feed/");



	//auto ps = new db::PreparedStatement("INSERT INTO one (sint, sstring, ssbool) VALUES (?,?,?)");
	//ps->set_int32(0, 456);
	//ps->set_string(1, "this is also a string");
	//ps->set_bool(2, false);

	//conn->Execute(ps);
	return 0;
}