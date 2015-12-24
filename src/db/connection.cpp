#include "connection.h"
#include "prepared_statement.h"

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
		PBLOG_CRITICAL << "Could not initialise MySQL connection";
	}
	mysql_real_connect(mysql_, conn_info_.host.c_str(), conn_info_.user.c_str(),
		conn_info_.password.c_str(), conn_info_.database.c_str(), conn_info_.port, nullptr, 0);

	if (!mysql_) {
		PBLOG_CRITICAL << "Could not connect to MySQL database at " << conn_info_.host;
		PBLOG_CRITICAL << "MySQL error: " << mysql_error(mysql_);
		mysql_close(mysql_);
		//TODO: throw exception
	}
	else {
		PBLOG_INFO << "libmysqlclient: " << mysql_get_client_info();
		PBLOG_INFO << "MySQL server version: " << mysql_get_server_info(mysql_);
		PBLOG_INFO << "Connected to MySQL db at " << conn_info_.host;
	}
}

void Connection::Ping()
{
	mysql_ping(mysql_);
}

void Connection::Execute()
{
}


enum_field_types CToMySQLType(TPreparedStatementDataTypeKey key)
{
	using pst = TPreparedStatementDataTypeKey;

	switch (key) {
	case pst::Uint8:
	case pst::Int8:
		return MYSQL_TYPE_TINY;
		break;
	case pst::Int16:
	case pst::Uint16:
		return MYSQL_TYPE_SHORT;
		break;
	case pst::Int32:
	case pst::Uint32:
		return MYSQL_TYPE_LONG;
		break;
	case pst::Int64:
	case pst::Uint64:
		return MYSQL_TYPE_LONGLONG;
		break;
	case pst::Float:
		return MYSQL_TYPE_FLOAT;
		break;
	case pst::Double:
		return MYSQL_TYPE_DOUBLE;
		break;
	case pst::Null:
		return MYSQL_TYPE_NULL;
		break;
	default:
		return MYSQL_TYPE_NULL;
		break;
	}
}

void Connection::Execute(PreparedStatement * ps)
{
	ps->statement_ = mysql_stmt_init(mysql_);
	const char* sql = ps->query_.c_str();
	mysql_stmt_prepare(ps->statement_, sql, strlen(sql));

	ps->BindBuffers(mysql_stmt_param_count(ps->statement_));

	mysql_stmt_bind_param(ps->statement_, ps->bind_);
	mysql_stmt_execute(ps->statement_);

	//TODO: Fix memory leak
}

u64 db::Connection::ExecuteAndReturnID(PreparedStatement * ps)
{
	Execute(ps);
	return mysql_stmt_insert_id(ps->statement_);
}
