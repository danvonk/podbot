#pragma once
#include <exception>
#include <stdexcept>

class SQLException : public std::runtime_error
{
public:
	SQLException(const std::string& state) {

	}


	SQLException(const SQLException& e)
		: std::runtime_error(e.what())
		, sql_state_(e.sql_state_)
		, error_no_(error_no_)
	{
	}




protected:
	const std::string sql_state_;
	const int error_no_;
};