#include "response.h"

using namespace http;

Response::Response()
{
}

Response::~Response()
{
}

void Response::set_status_code(int code)
{
	status_code_ = code;
}

int Response::get_status_code() const
{
	return status_code_;
}

void Response::set_content(std::string && content)
{
	content_ = std::move(content);
}

const std::string Response::get_content() const
{
	return content_;
}

void Response::add_header(std::string && key, std::string && val)
{
	headers_.insert(std::pair<std::string, std::string>(std::forward<std::string>(key), std::forward<std::string>(val)));
}
