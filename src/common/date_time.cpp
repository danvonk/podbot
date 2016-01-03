#include "date_time.h"

DateTime::DateTime()
	: dt_(boost::local_time::local_sec_clock::local_time(boost::local_time::time_zone_ptr()))
{
	rfc2822_facet_ = new boost::local_time::local_time_input_facet("%a, %d %b %Y %H:%M:%S %q");
}

DateTime::~DateTime()
{
	//delete rfc2822_facet_;
}

void DateTime::ParseRFC2822(const std::string & ts)
{
	std::stringstream ss(ts);
	ss.imbue(std::locale(std::locale::classic(), rfc2822_facet_));
	ss >> dt_;
}

unsigned int DateTime::Day()
{
	return dt_.utc_time().date().day();
}

unsigned int DateTime::Month()
{
	return dt_.utc_time().date().month();
}

unsigned int DateTime::Year()
{
	return dt_.utc_time().date().year();
}

unsigned int DateTime::Hour()
{
	return dt_.utc_time().time_of_day().hours();
}

unsigned int DateTime::Minute()
{
	return dt_.utc_time().time_of_day().minutes();
}

unsigned int DateTime::Second()
{
	return dt_.utc_time().time_of_day().seconds();
}
