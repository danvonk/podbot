#include "gpodder.h"
#include "rss_parser.h"

using namespace http;

Gpodder::Gpodder(boost::asio::io_service& io, db::Connection& conn)
	: conn_(conn)
	, client_(new http::HttpClient(io))
	, rss_(new RssParser(*(client_.get()), conn))
{

}

Gpodder::~Gpodder()
{
}