#include "common.h"

#include "http/client.h"
#include "http/request.h"
#include <fstream>
#include <iterator>
#include <cassert>

#include "db/sql_exception.h"
#include "common/log.h"
#include "common/config.h"
#include "db/connection.h"
#include "db/prepared_statement.h"
#include "db/result.h"
#include "db/field.h"
#include <mysql.h>

#include "http/url.hpp"
#include "itunes.h"
#include "common/date_time.h"


#include <regex>
#include <time.h>

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
	try {
		conn->Open();
	}
	catch (const SQLException& e) {
		PBLOG_CRITICAL << "Database connection error: " << e.what();
		return EXIT_FAILURE;
	}
	

	//auto ps = std::make_unique<db::PreparedStatement>("INSERT INTO test(times) VALUES (?)");
	//
	//DateTime* dt = new DateTime();
	//dt->ParseRFC2822("Sun, 20 Dec 2015 21:12:16 +0000");

	//std::cout << "Time is " << dt->Hour() << ":" << dt->Minute() << ":" << dt->Second();
	//ps->set_date_time(0, dt);
	//conn->Execute(ps.get());

	MYSQL_TIME ts;
	memset(&ts, 0, sizeof(ts));
	ts.day = 20;
	ts.month = 12;
	ts.year = 2014;
	ts.hour = 21;
	ts.minute = 12;
	ts.second = 38;

	MYSQL_BIND bnd;
	memset(&bnd, 0, sizeof(bnd));
	MYSQL_STMT* stmt = mysql_stmt_init(conn->get_mysql());
	const char* sql = "INSERT INTO test(times) VALUES (?)";
	mysql_stmt_prepare(stmt, sql, strlen(sql));

	bnd.buffer_type = MYSQL_TYPE_DATETIME;
	bnd.buffer = &ts;
	bnd.is_null = 0;
	*bnd.length = sizeof(ts);

	mysql_stmt_bind_param(stmt, &bnd);
	mysql_stmt_execute(stmt);

	//delete dt;
	/*Itunes it(*conn);
	it.ParseRSS("http://frogpants.com/feed/");*/
	return 0;
}