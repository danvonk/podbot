#pragma once

#include "common.h"
#include "http/client.h"
#include "db/connection.h"

class RssParser;
class Gpodder {
public:
	Gpodder(boost::asio::io_service& io, db::Connection& conn);
	~Gpodder();

	void RetrieveTop1000Categories();
	void RetrieveTop1000PodcastsFromCategory();

private:
	db::Connection& conn_;
	std::unique_ptr<http::HttpClient> client_;
	std::unique_ptr<RssParser> rss_;

};