#include "connection.h"
#include "sql_exception.h"
#include "prepared_statement.h"

using namespace db;

Connection::Connection(ConfigMgr * cfg)
	: mysql_(nullptr)
{
	conn_info_.user = cfg->GetValue<std::string>("db_user", "root");
	conn_info_.password = cfg->GetValue<std::string>("db_pass", "");
	conn_info_.database = cfg->GetValue<std::string>("db_name", "dev");
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
	mysql_close(mysql_);
}

//Connect to the database with the information supplied to the constructor
//This will throw an SQLException if a connection cannot be made.
void Connection::Open()
{
	MYSQL* pMysqlInit;
	pMysqlInit = mysql_init(NULL);
	if (!pMysqlInit) {
		throw SQLException("Could not connect to MySQL database");
	}

	mysql_ = mysql_real_connect(pMysqlInit, conn_info_.host.c_str(), conn_info_.user.c_str(),
		conn_info_.password.c_str(), conn_info_.database.c_str(), conn_info_.port, nullptr, 0);

	if (!mysql_) {
		const char* error = mysql_error(pMysqlInit);
		mysql_close(pMysqlInit);
		throw SQLException(error);

	} else {
		PBLOG_INFO << "Successfully connected to MySQL db at: " << conn_info_.host;
		PBLOG_INFO << "libmysqlclient: " << mysql_get_client_info();
		PBLOG_INFO << "MySQL server version: " << mysql_get_server_info(mysql_);

	}
}

void Connection::Ping()
{
	mysql_ping(mysql_);
}

void Connection::Execute()
{
}


enum_field_types CToMySQLType(PreparedStatementType key)
{
	using pst = PreparedStatementType;

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
	InitialisePrepStatement(ps->query_, ps);
	ps->BindBuffers();
	mysql_stmt_bind_param(ps->statement_, ps->bind_);
	if (mysql_stmt_execute(ps->statement_) != 0) {
		PBLOG_CRITICAL << "Could not execute statement!\n";
		PBLOG_CRITICAL << mysql_stmt_error(ps->statement_);
	}
	//TODO: Fix memory leak
}

u64 Connection::ExecuteAndReturnID(PreparedStatement * ps)
{
	Execute(ps);
	return mysql_stmt_insert_id(ps->statement_);
}

void Connection::ExecuteQuery(const std::string & sql)
{
	if (!mysql_) {
		//throw exception
		throw SQLException("No valid MySQL context active.");
		return;
	}
	if (mysql_query(mysql_, sql.c_str())) {
//		u32 error_num = mysql_errno(mysql_);
		throw SQLException(mysql_error(mysql_));
	}
}

std::unique_ptr<Result> Connection::ReturnExecQuery(const std::string & sql)
{
	if (!mysql_) {
		//throw exception
		return nullptr;
	}

	if (!mysql_query(mysql_, sql.c_str())) {
		MYSQL_RES* res = mysql_store_result(mysql_);
		MYSQL_FIELD* fields = mysql_fetch_fields(res);
		u64 rowCount = mysql_affected_rows(mysql_);
		u32 fieldCount = mysql_field_count(mysql_);

		return std::make_unique<Result>(res, fields, rowCount, fieldCount);
	} else {
		throw SQLException(mysql_error(mysql_));
	}

}

void Connection::InitialisePrepStatement(const std::string &sql, PreparedStatement *ps) {
	ps->statement_ = mysql_stmt_init(mysql_);
	if (!ps->statement_) {
		PBLOG_CRITICAL << "Could not create statement. " << mysql_error(mysql_);
		PBLOG_CRITICAL << "SQL was: " << sql;
	} else {
		if (mysql_stmt_prepare(ps->statement_, sql.c_str(), strlen(sql.c_str()))) {
			PBLOG_CRITICAL << "Could not prepare statement. " << mysql_stmt_error(ps->statement_);
			PBLOG_CRITICAL << "SQL was " << sql;
			mysql_stmt_close(ps->statement_);
		}
	}
}
