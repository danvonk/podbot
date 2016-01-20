#pragma once
#include "sql_exception.h"
#include "common.h"
#include "common/config.h"

#include "result.h"
#include "prepared_statement.h"
#include <mysql.h>

namespace db {
	class PreparedStatement;

	struct ConnectionInfo {
		std::string user;
		std::string password;
		std::string database;
		std::string host;
		int port;
	};

	class Connection {
	public:
		Connection(ConfigMgr* cfg);
		Connection(ConnectionInfo&& connInfo);
		~Connection();

		void Open();
		void Ping();
		void Execute();

		void InitialisePrepStatement(const std::string& sql, PreparedStatement* ps);
		void Execute(PreparedStatement* ps);
		u64 ExecuteAndReturnID(PreparedStatement* ps);

		void ExecuteQuery(const std::string& sql);
		std::unique_ptr<Result> ReturnExecQuery(const std::string& sql);

		MYSQL* get_mysql() {
			return mysql_;
		}

		//Delete assignment operators as MYSQL* cannot be copied.
		Connection(Connection const& right) = delete;
		Connection& operator=(Connection const& right) = delete;
	private:
		MYSQL* mysql_;
		ConnectionInfo conn_info_;
	};
}