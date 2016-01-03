#pragma once
#include "common.h"

#include <boost/date_time.hpp>
#include <boost/date_time/local_time/local_time.hpp>

class DateTime {
public:
	DateTime();
	~DateTime();

	void ParseRFC2822(const std::string& ts);
	
	unsigned int Day();
	unsigned int Month();
	unsigned int Year();

	unsigned int Hour();
	unsigned int Minute();
	unsigned int Second();
private:
	boost::local_time::local_time_input_facet* rfc2822_facet_;
	boost::local_time::local_date_time dt_;

};