#include "request.h"

using namespace http;

Request::Request()
	: user_agent_("dv/1.00.0 (gzip)")
	, connect_timeout_(60)
	, read_timeout_(60)
	, retries_(5)
{
}

Request::~Request()
{
}

//Url * Request::get_url()
//{
//	return &url_;
//}


const std::string & Request::get_user_agent() const
{
	return user_agent_;
}

int Request::get_retries() const
{
	return retries_;
}

int Request::get_connect_timeout() const
{
	return connect_timeout_;
}

int Request::get_read_timeout()
{
	return read_timeout_;
}

//void Request::set_url(Url& url)
//{
//	url_ = url;
//}

void Request::set_retries(int retries)
{
	retries_ = retries;
}

void Request::set_connect_timeout(int timeout)
{
	connect_timeout_ = timeout;
}

void Request::set_read_timeout(int timeout)
{
	read_timeout_ = timeout;
}

//void http::Request::set_url(Url && url)
//{
//	url_ = std::move(url);
//}

void http::Request::set_url(Url url)
{
	url_ = url;
}
