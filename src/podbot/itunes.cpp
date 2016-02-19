#include "itunes.h"
#include <gumbo.h>
#include "common/rapidjson/document.h"
#include "rss_parser.h"

using namespace http;
using namespace rapidjson;


void search_for_links(GumboNode* node, std::vector<std::string>& container) {
	if (node->type != GUMBO_NODE_ELEMENT) {
		return;
	}
	GumboAttribute* href;
	if (node->v.element.tag == GUMBO_TAG_A) {
		href = gumbo_get_attribute(&node->v.element.attributes, "href");
		container.push_back(href->value);
	}
	GumboVector* children = &node->v.element.children;
	for (u32 i = 0; i < children->length; ++i) {
		search_for_links(static_cast<GumboNode*>(children->data[i]), container);
	}
}

Itunes::Itunes(boost::asio::io_service& io, db::Connection& conn)
	: conn_(conn)
	, client_(new HttpClient(io))
	, exp_("^https?:\\/\\/itunes\\.apple\\.com\\/us\\/podcast\\/.+\\/id(\\d+)(?:\\?mt=2)?$")
	, rss_(new RssParser(*(client_.get()), conn))
{
}

Itunes::~Itunes() {

}

void Itunes::Categories() {
	Request req;
	req.set_url(Url("https://itunes.apple.com/us/genre/podcasts/id26?mt=2"));

	auto res = client_->make_request(&req);

	//search for every link and put them into a vector
	GumboOutput* output = gumbo_parse(res.content_.c_str());
	std::vector<std::string> links;
	search_for_links(output->root, links);

	std::regex exp("^(https?:\\/\\/itunes.apple.com\\/us\\/genre\\/podcasts-.+\\/id\\d+)\\?mt=2$");

	for (const auto& i : links) {
		std::smatch matches;
		if (std::regex_match(i, matches, exp)) {
			categories_.push_back(matches[1].str());
		}
	}

	PBLOG_INFO << "Found " << categories_.size() << " categories.";
	gumbo_destroy_output(&kGumboDefaultOptions, output);
}



void Itunes::Podcasts(const std::string& category) {
	for (int i = 0; i < 27; ++i) {
		Url url(category);
		url.add_query("mt", "2");
		url.add_query("letter");
		url.add_query("page");

		if (i == 26) {
			url.set_query(1).val("*");
		}
		else {
			url.set_query(1).val(std::string(1, static_cast<char>(i + 65)));
		}

		int page = 1;
		while (true) {
			url.set_query(2).val(std::to_string(page));
			std::cout << "requesting: " << url.str() << "\n";
			Request req;
			req.set_url(url);
			Response res = client_->make_request(&req);

			GumboOutput* output = gumbo_parse(res.content_.c_str());
			std::vector<std::string> links;
			search_for_links(output->root, links);

			int foundSize = 0;
			for (const auto& j : links) {
				std::smatch matches;
				if (std::regex_match(j, matches, exp_)) {
					rss_->ParseRSS(ResolveFeedUrl(matches[1].str()));
					foundSize++;
				}
			}
			if (foundSize < 2) {
				break; //there are no more pages
			}
			page++;
		}
		PBLOG_INFO << "Finished parsing letter " << static_cast<char>(i + 65) << " with " << page - 1 << " pages.";
	}
	PBLOG_INFO << "Parsed Category: " << category;
}



std::string Itunes::ResolveFeedUrl(const std::string& id) {
	Url url("https://itunes.apple.com/lookup");
	url.add_query("id", id);

	Request req;
	req.set_url(url);

	auto res = client_->make_request(&req);

	if (res.status_code_ != 200) {
		PBLOG_DEBUG << "Resolver was supplied invalid feed url or the HTTP request could not be completed.";
		return "";
	}

	Document doc;
	doc.Parse(res.content_.c_str());
	if (!doc.IsObject()) {
		PBLOG_DEBUG << "Resolver could not read JSON Response.";
		return "";
	}

	const auto& results = doc["results"];

	if (doc["resultCount"].GetUint() == 1) {
		return results[0u]["feedUrl"].GetString();
	}
	return "";
}

void Itunes::ParseAllCategories() {
	Podcasts(categories_[0]);
}