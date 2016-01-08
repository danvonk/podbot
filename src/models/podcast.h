#pragma once
#include "common.h"

#include "db/connection.h"
#include "db/prepared_statement.h"

class Podcast {
public:
	Podcast(db::Connection& conn);
	~Podcast();

	void set_title(const std::string& title);
	void set_link(const std::string& link);
	void set_description(const std::string& desc);
	void set_language(const std::string& lang);
	void set_category(const std::string& cat);
	void set_itunes_author(const std::string& author);
	void set_itunes_explicit(const std::string& expl);
	void set_copyright(const std::string& copyright);
	void set_itunes_image(const std::string& image);

	void Save();
	int SaveAndReturnID();

private:
	db::Connection& conn_;
	std::unique_ptr<db::PreparedStatement> stmt_;

	std::string title_;
	std::string link_;
	std::string description_;
	std::string language_;
	std::string category_;
	std::string itunes_author_;
	std::string itunes_explicit_;
	std::string copyright_;
	std::string itunes_image_;
};