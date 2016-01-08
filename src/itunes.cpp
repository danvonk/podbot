#include "itunes.h"
#include "http/client.h"

#include <regex>

#include "common/tinyxml2.h"
#include "common/rapidjson/rapidjson.h"
#include "common/rapidjson/document.h"

#include "db/prepared_statement.h"
#include "models/podcast.h"
#include "models/episode.h"


using namespace http;
using namespace rapidjson;
using namespace tinyxml2;


Itunes::Itunes(boost::asio::io_service& io, db::Connection& conn)
	: conn_(conn)
{
	client_ = std::make_unique<http_client>(io);
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
	std::string content = res->get_content();
	Document doc;
	doc.Parse(content.c_str());
	assert(doc.IsObject());
	std::cout << "result count : " << doc["resultCount"].GetInt() << "\n";
	auto& results = doc["results"];

	if (doc["resultCount"].GetUint() == 1){
		ParseRSS(results[0u]["feedUrl"].GetString());
	}
}

std::string returnBlankIfNull(XMLElement* el) {
	if (!el) {
		return "";
	}
	else {
		return el->GetText();
	}
}

std::vector<std::string>& split(const std::string& s, char delim, std::vector<std::string>& elms) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        if (!item.empty()) {
            elms.push_back(item);
        }
    }
    return elms;

}

u64 hmsToSeconds(const std::string& hms) {
    if (!hms.empty()){
        std::tm t;
        std::istringstream iss(hms);
        iss >> std::get_time(&t, "%H:%M:%S");
        return (t.tm_hour * 3600 + t.tm_min * 60 + t.tm_sec);
    }
    return 0;
}

void Itunes::ParseRSS(const std::string& feedUrl) {
	auto req = std::make_shared<Request>();
	req->set_url(Url(feedUrl));
    std::unique_ptr<Response> res;
    try {
        res = client_->Req(req.get());
    } catch (const std::exception& e) {
        PBLOG_ERROR << "HTTP Error: " << e.what();
        return;
    }

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

    const char* itunesImage = channel->FirstChildElement("itunes:image")->Attribute("href");
    if (itunesImage){
        pod.set_itunes_image(itunesImage);
    }

    int podcastID = pod.SaveAndReturnID();

	for (XMLElement* item = channel->FirstChildElement("item"); item != nullptr; item = item->NextSiblingElement("item")) {
		Episode ep(conn_);
        ep.set_podcast_id(podcastID);
        ep.set_title(returnBlankIfNull(item->FirstChildElement("title")));
        ep.set_link(returnBlankIfNull(item->FirstChildElement("link")));
        ep.get_pub_date()->ParseRFC2822(returnBlankIfNull(item->FirstChildElement("pubDate")));
        ep.set_description(returnBlankIfNull(item->FirstChildElement("description")));
        ep.set_itunes_subtitle(returnBlankIfNull(item->FirstChildElement("itunes:subtitle")));
        ep.set_itunes_summary(returnBlankIfNull(item->FirstChildElement("itunes:summary")));
        ep.set_itunes_author(returnBlankIfNull(item->FirstChildElement("itunes:author")));
        ep.set_itunes_duration(hmsToSeconds(item->FirstChildElement("itunes:duration")->GetText()));
        ep.set_guid(returnBlankIfNull(item->FirstChildElement("guid")));
        ep.Save();
	}

    db::PreparedStatement indexedFeed("INSERT INTO indexed_feeds(podcast_id,feed_url,created_at,updated_at) VALUES (?,?,?,?)");
    indexedFeed.set_uint32(0, podcastID);
    indexedFeed.set_string(1, feedUrl);
    indexedFeed.set_string(2, "now()");
    indexedFeed.set_string(3, "now()");



}