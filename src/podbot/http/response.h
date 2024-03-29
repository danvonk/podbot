#pragma once

#include "common.h"
#include "http_parser.h"
#include <map>

namespace http {
	//class Response {
	//	friend class HttpClient;
	//public:
	//	Response();
	//	~Response();

	//	void set_status_code(int code);
	//	int get_status_code() const;

	//	void set_content(std::string&& content);
	//	const std::string get_content() const;
	//	void add_header(std::string&& key, std::string&& val);
	//private:
	//	std::map<std::string, std::string> headers_;
	//	int status_code_;
	//	std::string content_;

	//};

	struct Response {
		std::map<std::string, std::string> headers_;
		int status_code_;
		std::string content_;
	};
	
}