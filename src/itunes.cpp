#include "itunes.h"
#include "http/client.h"

#include <regex>

#include "common/tinyxml2.h"
#include "common/rapidjson/rapidjson.h"
#include "common/rapidjson/document.h"

#include "models/podcast.h"


using namespace http;
using namespace rapidjson;
using namespace tinyxml2;


Itunes::Itunes(db::Connection& conn) 
	: conn_(conn)
{
	client_ = std::make_unique<http_client>();
}

Itunes::~Itunes() {

}


void Itunes::Categories() {
	auto req = std::make_shared <Request>();
	//req->set_url(Url("https://itunes.apple.com/us/genre/podcasts/id26?mt=2"));
	Url url("https://itunes.apple.com/us/genre/podcasts/id26?mt=2");
	req->set_url(url);

	auto res = client_->Req(req.get());

	//search for every link and put them into a vector
	GumboOutput* output = gumbo_parse(res->get_content().c_str());
	std::vector<std::string> links;
	search_for_links(output->root, links);

	std::regex exp("^(https?:\\/\\/itunes.apple.com\\/us\\/genre\\/podcasts-.+\\/id\\d+)\\?mt=2$");

	for (const auto& i : links) {
		std::smatch matches;
		if (std::regex_match(i, matches, exp)) {
			categories_.push_back(matches[1].str());
		}
	}

	gumbo_destroy_output(&kGumboDefaultOptions, output);
	Podcasts(categories_[0]);
}

void Itunes::search_for_links(GumboNode* node, std::vector<std::string>& container) {
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
			auto req = std::make_shared<Request>();
			req->set_url(url);
			auto res = client_->Req(req.get());

			GumboOutput* output = gumbo_parse(res->get_content().c_str());
			std::vector<std::string> links;
			search_for_links(output->root, links);

			std::regex exp("^https?:\\/\\/itunes\\.apple\\.com\\/us\\/podcast\\/.+\\/id(\\d+)(?:\\?mt=2)?$");
			//we need a temporary vector to avoid putting duplicates into the main vector, because of the end of page
			//check
			std::vector<std::string> found;

			for (const auto& j : links) {
				std::smatch matches;
				if (std::regex_match(j, matches, exp)) {
					found.push_back(matches[1].str());
					ParseFeed(matches[1].str());
				}
			}
			if (found.size() == 1 || found.size() == 0) {
				break; //there are no more pages
			}
			//merge the vectors
			podcasts_.insert(std::end(podcasts_), std::begin(found), std::end(found));
			page++;
		}
		std::cout << "Finished parsing letter " << static_cast<char>(i + 65) << " with " << page - 1 << "pages.\n";
	}
}

void Itunes::ParseFeed(const std::string& podcast) {
	Url url("https://itunes.apple.com/lookup");
	url.add_query("id", podcast);

	auto req = std::make_shared<Request>();
	req->set_url(url);

	auto res = client_->Req(req.get());

	/*Document json;
	json.Parse<0>(res->get_content().c_str());
	std::cout << res->get_content();
	
	auto& val = json["resultCount"];

	std::cout << "Got feed url " << val.GetString() << "\n";
	std::cout << "results. " << val.GetString() << "\n";
	ParseRSS(val.GetString());*/
	std::cout << podcast << "\n";

	std::string content = res->get_content();
	std::cout << content;

	Document doc;
	doc.Parse(content.c_str());
	assert(doc.IsObject());
	std::cout << "result count : " << doc["resultCount"].GetInt() << "\n";
	auto& results = doc["results"];

	ParseRSS(results[0u]["feedUrl"].GetString());
}

std::string returnBlankIfNull(XMLElement* el) {
	if (!el) {
		return "";
	}
	else {
		return el->GetText();
	}
}
void Itunes::ParseRSS(const std::string& feedUrl) {
	auto req = std::make_shared<Request>();
	req->set_url(Url(feedUrl));
	auto res = client_->Req(req.get());

	XMLDocument rssFeed;
	rssFeed.Parse(res->get_content().c_str());

	XMLElement* rss = rssFeed.FirstChildElement("rss");
	XMLElement* channel = rss->FirstChildElement("channel");

	Podcast pod(conn_);

	pod.set_title(returnBlankIfNull(channel->FirstChildElement("title")));
	pod.set_link(returnBlankIfNull(channel->FirstChildElement("link")));
	pod.set_description(returnBlankIfNull(channel->FirstChildElement("description")));
	pod.set_language(returnBlankIfNull(channel->FirstChildElement("language")));
	pod.set_category(returnBlankIfNull(channel->FirstChildElement("category")));
	pod.set_itunes_author(returnBlankIfNull(channel->FirstChildElement("itunes:author")));
	pod.set_itunes_explicit(returnBlankIfNull(channel->FirstChildElement("itunes:explicit")));
	pod.set_copyright(returnBlankIfNull(channel->FirstChildElement("copyright")));
	pod.set_itunes_image("test123");
	
	int podcastid = pod.SaveAndReturnID();


	for (XMLElement* item = channel->FirstChildElement("item"); item != nullptr; item = item->NextSiblingElement("item")) {
		//auto ps = std::make_unique<db::PreparedStatement>();
		//ps->set_uint32(0, podcastid);

		std::cout << "Title: " << returnBlankIfNull(item->FirstChildElement("title")) << "\n";
		std::cout << "Link: " << returnBlankIfNull(item->FirstChildElement("link")) << "\n";
		std::cout << "pubDate: " << returnBlankIfNull(item->FirstChildElement("pubDate"))<< "\n";
		std::cout << "description: " << returnBlankIfNull(item->FirstChildElement("description")) << "\n";
		std::cout << "sub: " << returnBlankIfNull(item->FirstChildElement("itunes:subtitle")) << "\n";
		std::cout << "summary: " << returnBlankIfNull(item->FirstChildElement("itunes:summary")) << "\n";
		std::cout << "author: " << returnBlankIfNull(item->FirstChildElement("itunes:author")) << "\n";
		std::cout << "\n";
	}

}