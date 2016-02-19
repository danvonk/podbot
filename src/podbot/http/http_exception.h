#pragma once
#include <exception>
#include <stdexcept>

namespace http {
	class HTTPException : public std::runtime_error
	{
	public:

		HTTPException(const std::string& state)
			: runtime_error(state)
			, error_(state)
		{}

		HTTPException(int error_num, const std::string& state)
			: runtime_error(state)
			, error_(state)

		{}

	protected:
		const std::string error_;
	};
}