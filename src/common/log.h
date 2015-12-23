#pragma once
#include "common.h"
#include <boost/log/attributes.hpp>
#include <boost/log/attributes/scoped_attribute.hpp>
#include <boost/log/common.hpp>


// Here we define our application severity levels.
enum class SeverityLevel
{
	Debug,
	Info,
	Warning,
	Error,
	Critical
};

// The formatting logic for the severity level
template< typename CharT, typename TraitsT >
inline std::basic_ostream< CharT, TraitsT >& operator<< (std::basic_ostream< CharT, TraitsT >& strm, SeverityLevel lvl)
{
	static const char* const str[] =
	{
		"Debug",
		"Info",
		"Warning",
		"Error",
		"Critical"
	};
	if (static_cast< std::size_t >(lvl) < (sizeof(str) / sizeof(*str)))
	{
		strm << str[static_cast<int>(lvl)];
	}
	else {
		strm << static_cast< int >(lvl);
	}
	return strm;
}


using namespace std;
class Log
{
private:
	struct Impl;
	Impl* impl;

public:
	typedef char char_type;

	Log();
	Log(Log const&);
	Log(Log&& that) noexcept : impl(that.impl) { that.impl = nullptr; }
	~Log();

	Log& operator= (Log const&);
	Log& operator= (Log&& that) noexcept
	{
		Log copy(static_cast< Log&& >(that)); // you can use use std::move here and include <utility>
		this->swap(copy);
		return *this;
	}
	
	std::pair<boost::log::attribute_set::iterator, bool> add_attribute(boost::log::attribute_name const&, boost::log::attribute const&);
	void remove_attribute(boost::log::attribute_set::iterator);
	void remove_all_attributes();
	boost::log::attribute_set get_attributes() const;
	void set_attributes(boost::log::attribute_set const&);

	boost::log::record open_record();
	boost::log::record open_record(SeverityLevel sev);
	template< typename Args >
	boost::log::record open_record(Args const& args)
	{
		return open_record(args[boost::log::keywords::severity]);
	}
	void push_record(boost::log::record&& rec);
	void swap(Log& that) noexcept
	{
		Impl* p = impl;
		impl = that.impl;
		that.impl = p;
	}
};

#define PBLOG(logger, lvl) BOOST_LOG_STREAM_SEV(logger, lvl)
#define PBLOG_DEBUG(logger) PBLOG(logger, SeverityLevel::Debug)
#define PBLOG_INFO(logger) PBLOG(logger, SeverityLevel::Info)
#define PBLOG_WARNING(logger) PBLOG(logger, SeverityLevel::Warning)
#define PBLOG_ERROR(logger) PBLOG(logger, SeverityLevel::Error)
#define PBLOG_CRITICAL(logger) PBLOG(logger, SeverityLevel::Critical)