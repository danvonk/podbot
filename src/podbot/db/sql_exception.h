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
		, error_(state)
		, error_no_(error_num)

	{}

protected:
	const std::string error_;
	const int error_no_;
};