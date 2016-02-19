#include "rss_parser.h"

#include "models/podcast.h"
#include "models/episode.h"

#include "common/tinyxml2.h"
#include "common/rapidjson/rapidjson.h"
#include "common/rapidjson/document.h"

using namespace tinyxml2;
using namespace http;

RssParser::RssParser(http::HttpClient& client, db::Connection& conn)
	: client_(client)
	, conn_(conn)
{
}

RssParser::~RssParser() {

}

std::string returnBlankIfNull(XMLElement* el) {
	if (!el) {
		return "";
	}
	else {
		if (el->GetText() && strlen(el->GetText()) != 0) {
			return el->GetText();
		}
		else {
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
		}
		else {
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
	if (!hms.empty()) {
		int h, m, s = 0;
		if (sscanf(hms.data(), "%d:%d:%d", &h, &m, &s) >= 2)
		{
			return u64(h * 3600 + m * 60 + s);
		}
	}
	return 0;
}

void RssParser::ParseRSS(const std::string& feedUrl) {
	if (feedUrl == "") {
		return;
	}

	Request req;
	req.set_url(Url(feedUrl));

	Response res;
	try {
		res = client_.make_request(&req);
	}
	catch (const std::exception& e) {
		PBLOG_ERROR << "HTTP Error: " << e.what();
		return;
	}

	XMLDocument rssFeed;
	rssFeed.Parse(res.content_.c_str());

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
	if (itunesImage) {
		if (itunesImage->Attribute("href")) {
			pod.set_itunes_image(itunesImage->Attribute("href"));
		}
	}

	int podcastID = pod.SaveAndReturnID();
	Episode ep(conn_);

	for (XMLElement* item = channel->FirstChildElement("item"); item != nullptr; item = item->NextSiblingElement("item")) {
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
		ep.Clear();
	}

	std::stringstream sss;
	sss << "INSERT INTO indexed_feeds(podcast_id,feed_url,created_at,updated_at) VALUES (";
	sss << std::to_string(podcastID);
	sss << ",\"";
	sss << feedUrl;
	sss << "\", now(), now())";
	conn_.ExecuteQuery(sss.str());
	PBLOG_DEBUG << "Parsed Feed: " << feedUrl;
}
