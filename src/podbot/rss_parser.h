#pragma once
#include "common.h"
#include "http/client.h"
#include "db/connection.h"


class RssParser {
public:
	RssParser(http::HttpClient& client, db::Connection& conn);
	~RssParser();

	void ParseRSS(const std::string& feedUrl);

private:
	http::HttpClient& client_;
	db::Connection& conn_;
};