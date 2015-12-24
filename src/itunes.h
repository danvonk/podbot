#pragma once
#include "common.h"

class Itunes {
public:
	Itunes();
	~Itunes();

	void Categories();
	void Podcasts(const std::string& category);
	void ParseFeed(const std::string& podcast);
	void ParseRSS();
private:
	std::vector<std::string> categories_;
	std::vector<std::string> podcasts_; //the itunes url of each podcast

};