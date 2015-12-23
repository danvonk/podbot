#pragma once
#include "common.h"
#include "common/config.h"
#include <mysql.h>

struct ConnectionInfo {
	std::string user;
	std::string password;
	std::string database;
	std::string host;
	int port;
};


namespace db {
	class Connection {
	public:
		Connection(ConfigMgr* cfg);
		Connection(ConnectionInfo&& connInfo);
		~Connection();

		void Open();
		void Ping();
		void Execute();

		//Delete assignment operators as MYSQL* cannot be copied.
		Connection(Connection const& right) = delete;
		Connection& operator=(Connection const& right) = delete;
	private:
		Log log_;
		MYSQL* mysql_;
		ConnectionInfo conn_info_;
	};
}