#include "log.h"
#include "common.h"

#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/core/null_deleter.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <boost/log/expressions.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>
#include <boost/log/support/date_time.hpp>

namespace logging = boost::log;
namespace attrs = boost::log::attributes;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;

using file_sink = sinks::synchronous_sink<sinks::text_file_backend>;
using text_sink = sinks::synchronous_sink<sinks::text_ostream_backend>;
using DefaultLogger = boost::log::sources::severity_logger<SeverityLevel>;

struct Log::Impl
{
	DefaultLogger lg;
};

Log::Log() 
	: impl(new Impl())
{
	boost::shared_ptr<text_sink> tsink(new text_sink);
	boost::shared_ptr<std::ostream> pStream(&std::clog, boost::null_deleter());
	tsink->locked_backend()->add_stream(pStream);
	tsink->set_formatter(expr::format("[%1%][%2%]: %3%")
		% expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%d/%m/%Y %H:%M:%S")
		% expr::attr<SeverityLevel>("Severity")
		% expr::smessage);
	logging::core::get()->add_sink(tsink);

	//TODO: This has failed on Windows 7 before...

	boost::shared_ptr<file_sink> fsink(new file_sink(
		keywords::file_name = "%Y%m%d_pb.log",
		keywords::rotation_size = 16384
		));
	fsink->locked_backend()->set_file_collector(sinks::file::make_collector(
		keywords::target = "../res/log",
		keywords::max_size = 16 * 1024 * 1024,
		keywords::min_free_space = 100 * 1024 * 1024
		));
	fsink->locked_backend()->scan_for_files();

	fsink->set_formatter(expr::format("[%1%][%2%]: %3%")
		% expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%d/%m/%Y %H:%M:%S")
		% expr::attr<SeverityLevel>("Severity")
		% expr::smessage);
	logging::core::get()->add_sink(fsink);

	logging::core::get()->add_global_attribute("TimeStamp", attrs::local_clock());
	logging::core::get()->set_filter(expr::attr<SeverityLevel>("Severity").or_none() >= SeverityLevel::Debug);
}

Log::Log(Log const& that) : impl(new Impl(*that.impl))
{
}

Log::~Log()
{
	delete impl;
}

Log& Log::operator= (Log const& that)
{
	Log(that).swap(*this);
	return *this;
}

std::pair<boost::log::attribute_set::iterator, bool> Log::add_attribute(boost::log::attribute_name const & name, boost::log::attribute const & attr)
{
	return impl->lg.add_attribute(name, attr);
}

void Log::remove_attribute(boost::log::attribute_set::iterator it)
{
	impl->lg.remove_attribute(it);
}

void Log::remove_all_attributes()
{
	impl->lg.remove_all_attributes();
}

boost::log::attribute_set Log::get_attributes() const
{
	return impl->lg.get_attributes();
}

void Log::set_attributes(boost::log::attribute_set const & attrs)
{
	impl->lg.set_attributes(attrs);
}

boost::log::record Log::open_record()
{
	return impl->lg.open_record();
}

boost::log::record Log::open_record(SeverityLevel sev)
{
	return impl->lg.open_record((boost::log::keywords::severity = sev));
}

void Log::push_record(boost::log::record&& rec)
{
	impl->lg.push_record(std::move(rec));
}