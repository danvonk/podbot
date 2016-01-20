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
//					found.push_back(matches[1].str());
					ParseRSS(ResolveFeedUrl(matches[1].str()));
				}
			}
			if (found.size() == 1 || found.size() == 0) {
				break; //there are no more pages
			}
			//merge the vectors
//			podcasts_.insert(std::end(podcasts_), std::begin(found), std::end(found));
			page++;
		}
		std::cout << "Finished parsing letter " << static_cast<char>(i + 65) << " with " << page - 1 << "pages.\n";
	}
	PBLOG_INFO << "Parsed Category: " << category;
}



std::string Itunes::ResolveFeedUrl(const std::string& id) {
	Url url("https://itunes.apple.com/lookup");
	url.add_query("id", id);

	auto req = std::make_shared<Request>();
	req->set_url(url);

	auto res = client_->Req(req.get());
	std::string content = res->get_content();
	Document doc;
	doc.Parse(content.c_str());
	if (!doc.IsObject()) {
		return "";
	}

	const auto& results = doc["results"];

	if (doc["resultCount"].GetUint() == 1){
		return results[0u]["feedUrl"].GetString();
	}
	return "";
}

std::string returnBlankIfNull(XMLElement* el) {
	if (!el) {
		return "";
	}
	else {
		if (el->GetText() && strlen(el->GetText()) != 0){
			return el->GetText();
		} else {
			return std::string();
		}
	}
}

//like the above function, just for returning attribute values instead.
std::string returnAttribute(XMLElement* el, const char* name) {
	if (!el) {
		return "";
	}
	else {
		if (el->Attribute(name) != 0) {
			return el->Attribute(name);
		} else {
			return std::string();
		}
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
		int h, m, s= 0;
		if (sscanf(hms.data(), "%d:%d:%d", &h, &m, &s) >= 2)
		{
			return u64(h  * 3600 + m * 60 + s);
		}
    }
    return 0;
}

void Itunes::ParseRSS(const std::string& feedUrl) {
	if (feedUrl == "") {
		return;
	}

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
	if (!rss) {
		return;
	}
	XMLElement* channel = rss->FirstChildElement("channel");
	if (!channel) {
		return;
	}

	Podcast pod(conn_);

	pod.set_title(returnBlankIfNull(channel->FirstChildElement("title")));
	pod.set_link(returnBlankIfNull(channel->FirstChildElement("link")));
	pod.set_description(returnBlankIfNull(channel->FirstChildElement("description")));
	pod.set_language(returnBlankIfNull(channel->FirstChildElement("language")));
	pod.set_category(returnBlankIfNull(channel->FirstChildElement("category")));
	pod.set_itunes_author(returnBlankIfNull(channel->FirstChildElement("itunes:author")));
	pod.set_itunes_explicit(returnBlankIfNull(channel->FirstChildElement("itunes:explicit")));
	pod.set_copyright(returnBlankIfNull(channel->FirstChildElement("copyright")));

    XMLElement* itunesImage = channel->FirstChildElement("itunes:image");
    if (itunesImage){
		if (itunesImage->Attribute("href")) {
			pod.set_itunes_image(itunesImage->Attribute("href"));
		}
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

		if (item->FirstChildElement("itunes:summary")) {
			if (item->FirstChildElement("itunes:summary")->FirstChild()) {
				ep.set_itunes_summary(item->FirstChildElement("itunes:summary")->FirstChild()->Value());
			}
		}

        ep.set_itunes_author(returnBlankIfNull(item->FirstChildElement("itunes:author")));
        ep.set_itunes_duration(hmsToSeconds(returnBlankIfNull(item->FirstChildElement("itunes:duration"))));
        ep.set_guid(returnBlankIfNull(item->FirstChildElement("guid")));

		XMLElement* enclosure = item->FirstChildElement("enclosure");
		if (!enclosure) {
			break;
		}


		ep.set_enclosure_url(returnAttribute(enclosure, "url"));
		ep.set_enclosure_length(returnAttribute(enclosure, "length"));
		ep.set_enclosure_type(returnAttribute(enclosure, "type"));

		ep.Save();
	}

    auto indexedFeed = std::make_unique<db::PreparedStatement>("INSERT INTO indexed_feeds(podcast_id,feed_url,created_at,updated_at) VALUES (?,?,?,?)");
    indexedFeed->set_uint32(0, podcastID);
    indexedFeed->set_string(1, feedUrl);
    indexedFeed->set_string(2, "now()");
    indexedFeed->set_string(3, "now()");
	conn_.Execute(indexedFeed.get());

}

void Itunes::ParseAllCategories() {
	for (const auto& cat : categories_) {
		Podcasts(cat);
	}
}
