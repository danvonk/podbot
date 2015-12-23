#include "connection.h"

using namespace db;

Connection::Connection(ConfigMgr * cfg)
	: mysql_(nullptr)
{
	conn_info_.user = cfg->GetValue<std::string>("db_user", "root");
	conn_info_.password = cfg->GetValue<std::string>("db_pass", "");
	conn_info_.database = cfg->GetValue<std::string>("db_name", "test");
	conn_info_.host = cfg->GetValue<std::string>("db_host", "localhost");
	conn_info_.port = cfg->GetValue<int>("db_port", 3306);
}

Connection::Connection(ConnectionInfo && connInfo)
	: mysql_(nullptr)
	, conn_info_(std::move(connInfo))
{
}

db::Connection::~Connection()
{
}

void Connection::Open()
{
	mysql_ = mysql_init(nullptr);
	if (!mysql_) {
		PBLOG_CRITICAL(log_) << "Could not initialise MySQL connection";
	}
	mysql_real_connect(mysql_, conn_info_.host.c_str(), conn_info_.user.c_str(),
		conn_info_.password.c_str(), conn_info_.database.c_str(), conn_info_.port, nullptr, 0);

	if (!mysql_) {
		PBLOG_CRITICAL(log_) << "Could not connect to MySQL database at " << conn_info_.host;
		PBLOG_CRITICAL(log_) << "MySQL error: " << mysql_error(mysql_);
		mysql_close(mysql_);
		//TODO: throw exception
	}
	else {
		//PBLOG_INFO(log_) << "libmysqlclient: " << mysql_get_client_info();
		//PBLOG_INFO(log_) << "MySQL server version: " << mysql_get_server_info(mysql_);
		PBLOG_INFO(log_) << "Connected to MySQL db at " << conn_info_.host;
	}
}

void Connection::Ping()
{
	mysql_ping(mysql_);
}

void Connection::Execute()
{
}
