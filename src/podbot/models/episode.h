#pragma once


#include "common.h"
#include "common/date_time.h"
#include "db/connection.h"
#include "db/prepared_statement.h"

class Episode {
public:
    Episode(db::Connection& conn);
    ~Episode();

    void Save();
    int SaveAndReturnId();

    const std::string &get_title() const {
        return title_;
    }

    void set_title(const std::string &title) {
        title_ = title;
    }

    int get_podcast_id() const {
        return podcast_id_;
    }

    void set_podcast_id(int podcast_id) {
        podcast_id_ = podcast_id;
    }

    const std::string &get_link() const {
        return link_;
    }

    void set_link(const std::string &link) {
        link_ = link;
    }

    const std::string &get_description() const {
        return description_;
    }

    void set_description(const std::string &description) {
        description_ = description;
    }

    const std::string &get_itunes_subtitle() const {
        return itunes_subtitle_;
    }

    void set_itunes_subtitle(const std::string &itunes_subtitle) {
        itunes_subtitle_ = itunes_subtitle;
    }

    const std::string &get_itunes_summary() const {
        return itunes_summary_;
    }

    void set_itunes_summary(const std::string &itunes_summary) {
        itunes_summary_ = itunes_summary;
    }

    const std::string &get_itunes_author() const {
        return itunes_author_;
    }

    void set_itunes_author(const std::string &itunes_author) {
        itunes_author_ = itunes_author;
    }

    u64 get_itunes_duration() const {
        return itunes_duration_;
    }

    void set_itunes_duration(u64 itunes_duration) {
        itunes_duration_ = itunes_duration;
    }

    const std::string &get_itunes_explicit() const {
        return itunes_explicit_;
    }

    void set_itunes_explicit(const std::string &itunes_explicit) {
        itunes_explicit_ = itunes_explicit;
    }

    const std::string &get_enclosure_url() const {
        return enclosure_url_;
    }

    void set_enclosure_url(const std::string &enclosure_url) {
        enclosure_url_ = enclosure_url;
    }

    const std::string &get_enclosure_length() const {
        return enclosure_length_;
    }

    void set_enclosure_length(const std::string &enclosure_length) {
        enclosure_length_ = enclosure_length;
    }

    const std::string &get_enclosure_type() const {
        return enclosure_type_;
    }

    void set_enclosure_type(const std::string &enclosure_type) {
        enclosure_type_ = enclosure_type;
    }

    const std::string &get_guid() const {
        return guid_;
    }

    void set_guid(const std::string &guid) {
        guid_ = guid;
    }

    const std::string &get_itunes_image() const {
        return itunes_image_;
    }

    void set_itunes_image(const std::string &itunes_image) {
        itunes_image_ = itunes_image;
    }

    DateTime* get_pub_date(){
        return &pub_date_;
    }

private:
    db::Connection& conn_;
    std::unique_ptr<db::PreparedStatement> stmt_;

    std::string title_;
    int podcast_id_;
    std::string link_;

    DateTime pub_date_;

    std::string description_;
    std::string itunes_subtitle_;
    std::string itunes_summary_;
    std::string itunes_author_;
    u64 itunes_duration_;
    std::string itunes_explicit_;
    std::string enclosure_url_;
    std::string enclosure_length_;
    std::string enclosure_type_;
    std::string guid_;
    std::string itunes_image_;
};