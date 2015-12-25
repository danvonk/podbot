#pragma once
#include "common.h"
#include "http/client.h"
#include <gumbo.h>

#include "models/podcast.h"

class Itunes {
public:
	Itunes(db::Connection&  conn);
	~Itunes();


	//fill the categories vector with the url of each category
	void Categories();
	//fill the podcasts vector with the url of each podcast from a specified category
	void Podcasts(const std::string& category);
	void ParseFeed(const std::string& podcast);
	void ParseRSS(const std::string& feedUrl);
private:
	void search_for_links(GumboNode* node, std::vector<std::string>& container);

	std::vector<std::string> categories_;
	std::vector<std::string> podcasts_; //the itunes url of each podcast

	std::unique_ptr<http::http_client> client_;

	db::Connection& conn_;

};