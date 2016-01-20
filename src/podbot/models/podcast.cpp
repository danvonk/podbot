#include "podcast.h"

using namespace db;
#define PODCAST_CREATE_STMT "INSERT INTO podcasts (title, link, description, `language`, category, itunes_author, itunes_explicit, copyright, itunes_image, created_at, updated_at) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)" 

Podcast::Podcast(Connection& conn)
	: conn_(conn)
	, stmt_(new db::PreparedStatement(PODCAST_CREATE_STMT))
{
}

Podcast::~Podcast()
{
}

void Podcast::set_title(const std::string & title)
{
	title_ = title;
}

void Podcast::set_link(const std::string & link)
{
	link_ = link;
}

void Podcast::set_description(const std::string & desc)
{
	description_ = desc;
}

void Podcast::set_language(const std::string & lang)
{
	language_ = lang;
}

void Podcast::set_category(const std::string & cat)
{
	category_ = cat;
}

void Podcast::set_itunes_author(const std::string & author)
{
	itunes_author_ = author;
}

void Podcast::set_itunes_explicit(const std::string & expl)
{
	itunes_explicit_ = expl;
}

void Podcast::set_copyright(const std::string & copyright)
{
	copyright_ = copyright;
}

void Podcast::set_itunes_image(const std::string & image)
{
	itunes_image_ = image;
}

void Podcast::Save()
{
	stmt_->set_string(0, title_);
	stmt_->set_string(1, link_);
	stmt_->set_string(2, description_);
	stmt_->set_string(3, language_);
	stmt_->set_string(4, category_);
	stmt_->set_string(5, itunes_author_);
	stmt_->set_string(6, itunes_explicit_);
	stmt_->set_string(7, copyright_);
	stmt_->set_string(8, itunes_image_);
	stmt_->set_string(9, "now()");
	stmt_->set_string(10, "now()");

	conn_.Execute(stmt_.get());
}

int Podcast::SaveAndReturnID()
{
	stmt_->set_string(0, title_);
	stmt_->set_string(1, link_);
	stmt_->set_string(2, description_);
	stmt_->set_string(3, language_);
	stmt_->set_string(4, category_);
	stmt_->set_string(5, itunes_author_);
	stmt_->set_string(6, itunes_explicit_);
	stmt_->set_string(7, copyright_);
	stmt_->set_string(8, itunes_image_);
	stmt_->set_string(9, "now()");
	stmt_->set_string(10, "now()");

	return conn_.ExecuteAndReturnID(stmt_.get());
}
