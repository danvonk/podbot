#pragma once
#include "common.h"
#include "common/config.h"
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

		void Execute(PreparedStatement* ps);
		u64 ExecuteAndReturnID(PreparedStatement* ps);

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