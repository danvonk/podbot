#include "common.h"

#include "http/client.h"
#include <fstream>

#include "db/sql_exception.h"
#include "common/config.h"
#include "db/connection.h"
#include "db/prepared_statement.h"
#include "common/date_time.h"

#include "itunes.h"

using namespace http;

db::Connection* conn;
boost::asio::deadline_timer* mysqlTimer;

void keep_mysql_alive(const boost::system::error_code& err) {
    if (!err) {
        PBLOG_INFO << "Pinging MySQL";
        conn->Ping();

    }
}

int main(int argc, char* argv[]) {
	std::string configFile = "../res/podbot.cfg";

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

    boost::asio::io_service ioService;
    mysqlTimer = new boost::asio::deadline_timer(ioService);
    mysqlTimer->expires_from_now(boost::posix_time::minutes(cfgMgr->GetValue<int>("db_ping_time", 30)));
    mysqlTimer->async_wait(keep_mysql_alive);

	mysql_library_init(0, nullptr, nullptr);
	conn = new db::Connection(cfgMgr.get());
	try {
		conn->Open();
	}
	catch (const SQLException& e) {
		PBLOG_CRITICAL << "Database connection error: " << e.what();
		return EXIT_FAILURE;
	}

    Itunes it(ioService, *conn);
    it.ParseFeed("595464480");

	

    delete conn;
    delete mysqlTimer;
	return 0;
}