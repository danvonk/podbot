#include "common.h"

#include "http/client.h"
#include "http/request.h"
#include <fstream>
#include <iterator>
#include <cassert>

#include "common/log.h"
#include "common/config.h"
#include "db/connection.h"

#include <cstdlib>
//#include <mysql_connection.h>
//#include <cppconn/driver.h>
//#include <cppconn/exception.h>
//#include <cppconn/resultset.h>
//#include <cppconn/statement.h>
#include <mysql.h>

using namespace http;

int main(int argc, char* argv[]) {
	Log lg;
	std::string configFile = "C:/dev/podbot_asio/res/podbot.cfg";

	PBLOG_DEBUG(lg) << "    ____            ____          __ ";
	PBLOG_DEBUG(lg) << "   / __ \\____  ____/ / /_  ____  / /_";
	PBLOG_DEBUG(lg) << "  / /_/ / __ \\/ __  / __ \\/ __ \\/ __/";
	PBLOG_DEBUG(lg) << " / ____/ /_/ / /_/ / /_/ / /_/ / /_  ";
	PBLOG_DEBUG(lg) << "/_/    \\____/\\__,_/_.___/\\____/\\__/  ";
	PBLOG_DEBUG(lg) << "                                     ";
	PBLOG_DEBUG(lg) << "Config file: " << configFile;
	PBLOG_DEBUG(lg) << "OpenSSL version: " << OPENSSL_VERSION_TEXT;
	PBLOG_DEBUG(lg) << "Boost version: " << BOOST_VERSION / 100000 << "." << BOOST_VERSION / 100 % 1000 << "." << BOOST_VERSION % 100;
	PBLOG_DEBUG(lg) << "<Ctrl-C> to stop.";

	auto cfgMgr = std::make_unique<ConfigMgr>(lg);
	cfgMgr->ParseCommandLine(argc, argv);
	cfgMgr->ParseConfigFile(configFile);

	/*try {
		sql::Driver* dvr = get_driver_instance();
		auto connection = dvr->connect("tcp://127.0.0.1:3306", "root", "");
		connection->setSchema("test");
		if (!connection->isValid()) {
			PBLOG_CRITICAL(lg) << "Could not connect to database.";
		}

		auto stmnt = connection->createStatement();
		auto res = stmnt->executeQuery("SELECT * FROM one WHERE id='1'");
		while (res->next()) {
			PBLOG_INFO(lg) << "Result is " << res->getString(2);
		}

		delete res;
		delete stmnt;
		delete connection;
	}
	catch (const sql::SQLException& e) {
		PBLOG_ERROR(lg) << e.what();
		PBLOG_ERROR(lg) << "mysql error: " << e.getErrorCode() << "," << e.getSQLState();
	}
*/
	mysql_library_init(0, nullptr, nullptr);
	auto conn = new db::Connection(cfgMgr.get());
	conn->Open();

	/*if (mysql_query(pConn, "SELECT * FROM one WHERE id='1'")) {
		PBLOG_ERROR(lg) << "query error.";
	}

	MYSQL_RES* result = mysql_store_result(pConn);
	MYSQL_ROW row;
	while ((row = mysql_fetch_row(result))) {
		for (int i = 0; i < mysql_num_fields(result); ++i) {
			if (row[i] != nullptr) {
				PBLOG_INFO(lg) << row[i];
			}
		}
	}

	const char* stm = "INSERT INTO one VALUES (?)";
	MYSQL_STMT* prepStmt = mysql_stmt_init(pConn);
	mysql_stmt_prepare(prepStmt, stm, strlen(stm));*/



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


	return 0;
}