#include "date_time.h"
#include <chrono>
#include <sstream>
#include <ctime>

using namespace boost::local_time;

DateTime::DateTime()
//	: rfc2822_facet_("%a, %d %b %Y %H:%M:%S %q")
// 	, dt_(local_sec_clock::local_time(time_zone_ptr(new posix_time_zone("GMT"))))
{

}

DateTime::~DateTime()
{
	//delete rfc2822_facet_;
}

void DateTime::ParseRFC2822(const std::string & ts)
{
	std::istringstream ss(ts);
	ss >> std::get_time(&tm_, "%a, %d %b %Y %H:%M:%S") >> tz_;
}

unsigned int DateTime::Day()
{
//	return dt_.utc_time().date().day();
	return tm_.tm_mday;
}

unsigned int DateTime::Month()
{
	return tm_.tm_mon + 1;
}

unsigned int DateTime::Year()
{
	return tm_.tm_year + 1900;
}

unsigned int DateTime::Hour()
{
	return tm_.tm_hour;
}

unsigned int DateTime::Minute()
{
	return tm_.tm_min;
}

unsigned int DateTime::Second()
{
	return tm_.tm_sec;
}

void DateTime::Clear()
{
  memset(&tm_, 0, sizeof(tm_));
}
