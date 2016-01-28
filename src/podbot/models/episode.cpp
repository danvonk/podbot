//
// Created by dan on 1/6/16.
//

#include "episode.h"

using namespace db;
#define EPISODE_CREATE_STMT "INSERT INTO episodes(podcast_id,title,link,pubdate,description,itunes_subtitle,itunes_author,itunes_duration, itunes_summary, itunes_explicit,enclosure_url,enclosure_length,enclosure_type,guid,itunes_image,created_at, updated_at) VALUES (?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)"

Episode::Episode(Connection& conn)
    : conn_(conn)
    , stmt_(EPISODE_CREATE_STMT)
{
}

Episode::~Episode() {
}

void Episode::Save() {
    stmt_.set_int32(0, podcast_id_);
    stmt_.set_string(1, title_);
    stmt_.set_string(2, link_);
    stmt_.set_date_time(3, &pub_date_);
    stmt_.set_string(4, description_);
    stmt_.set_string(5, itunes_subtitle_);
    stmt_.set_string(6, itunes_author_);
    stmt_.set_uint64(7, itunes_duration_);
    stmt_.set_string(8, itunes_summary_);
    stmt_.set_string(9, itunes_explicit_);
    stmt_.set_string(10, enclosure_url_);
    stmt_.set_string(11, enclosure_length_);
    stmt_.set_string(12, enclosure_type_);
    stmt_.set_string(13, guid_);
    stmt_.set_string(14, itunes_image_);
    stmt_.set_string(15, "now()");
    stmt_.set_string(16, "now()");

    conn_.ExecuteStatement(&stmt_);
}

int Episode::SaveAndReturnId() {
    return 0;
}

void Episode::Clear()
{
  podcast_id_ = 0;
  title_.clear();
  link_.clear();
  description_.clear();
  itunes_subtitle_.clear();
  itunes_author_.clear();
  itunes_duration_ = 0;
  itunes_summary_.clear();
  itunes_explicit_.clear();
  enclosure_url_.clear();
  enclosure_length_.clear();
  enclosure_type_.clear();
  guid_.clear();
  itunes_image_.clear();
  pub_date_.Clear();
}
