#pragma once
#include <exception>
#include <stdexcept>

class SQLException : public std::runtime_error
{
public:

	SQLException(const std::string& state)
		: runtime_error(state)
		, error_(state)
		, error_no_(0)
	{}

	SQLException(int error_num, const std::string& state)
		: runtime_error(state)
		, error_no_(error_num)
		, error_(state)
	{}


	SQLException(const SQLException& e)
		: std::runtime_error(e.what())
		, error_(e.error_)
		, error_no_(error_no_)
	{
	}

protected:
	const std::string error_;
	const int error_no_;
};