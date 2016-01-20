#include "client.h"
#include <iterator>
#include <boost/asio/use_future.hpp>
#include <future>
#include "response.h"

using namespace http;
namespace asio = boost::asio;
using asio::ip::tcp;

http_parser_settings parser_settings_;


http_client::http_client(asio::io_service& io)
	//: logger_(log)
	: service_(io)
	, hps_(HeaderParserState::HeaderStart)
	, ssl_ctx_(asio::ssl::context::sslv23)
	, resolver_(service_)
	, socket_(service_)
	, ssl_socket_(service_, ssl_ctx_)
{
	ssl_ctx_.set_default_verify_paths();
	ssl_ctx_.load_verify_file("../res/cacert.pem");

	//http_parser_init(&parser_, HTTP_RESPONSE);

	parser_settings_.on_url = http_client::on_url_cb;
	parser_settings_.on_header_field = http_client::on_header_field_cb;
	parser_settings_.on_header_value = http_client::on_header_value_cb;
	parser_settings_.on_body = http_client::on_body_cb;
	parser_settings_.on_headers_complete = http_client::on_header_complete_cb;

	/*parser_.data = this;*/
}

std::unique_ptr<Response> http_client::Req(Request* req)
{
	bool usingSSL = false;
	auto res = std::make_unique<Response>();

	//initialise parser
	res->parser_.data = this;
	http_parser_init(&res->parser_, HTTP_RESPONSE);


	response_ptr_ = res.get();
	//try {
		if (req->get_url().scheme() == "https") {
			usingSSL = true;
		}

		std::ostream req_str(&request_);
		req_str << "GET " << req->get_url().pathForRequest() << " HTTP/1.0\r\n";
		req_str << "Host: " << req->get_url().host() << "\r\n";
		req_str << "Accept: */*\r\n";
		req_str << "Connection: close\r\n\r\n";

		//ssl_socket_.set_verify_callback(boost::asio::ssl::rfc2818_verification("danvonk.com"));
		tcp::resolver::query query(req->get_url().host(), req->get_url().scheme());
		std::ostringstream sstr;

		if (usingSSL) {
			asio::connect(ssl_socket_.lowest_layer(), resolver_.resolve(query));
			ssl_socket_.lowest_layer().set_option(tcp::no_delay(true));

			ssl_socket_.set_verify_mode(asio::ssl::verify_peer);
			ssl_socket_.set_verify_callback(boost::bind(&http_client::verify_certificate, this, _1, _2));

			try {
				ssl_socket_.handshake(asio::ssl::stream_base::client);
			} catch (const std::exception& e) {
				return res;
			}
			asio::write(ssl_socket_, request_);

			//read until eof
			boost::system::error_code error;
			while (asio::read(ssl_socket_, response_, asio::transfer_at_least(1), error)) {
				sstr << &response_;
			}
			if (error == asio::error::eof) {
				if (http_parser_execute(&res->parser_, &parser_settings_, nullptr, 0) != 0) {
					std::cerr << "Parsing error.\n";
				}
			}
			else {
				throw boost::system::system_error(error);
			}
		}
		else {
			asio::connect(socket_, resolver_.resolve(query));
			asio::write(socket_, request_);

			//read until eof
			boost::system::error_code error;
			while (asio::read(socket_, response_, asio::transfer_at_least(1), error)) {
				sstr << &response_;
			}
			if (error == asio::error::eof) {
				if (http_parser_execute(&res->parser_, &parser_settings_, nullptr, 0) != 0) {
					std::cerr << "Parsing error.\n";
				}
			}
			else {
				throw boost::system::system_error(error);
			}

		}

		size_t bytes_parsed = http_parser_execute(&res->parser_, &parser_settings_, sstr.str().c_str(), strlen(sstr.str().c_str()));
		res->set_status_code(res->parser_.status_code);

		if (usingSSL) {
			boost::system::error_code ec;
			ssl_socket_.lowest_layer().shutdown(tcp::socket::shutdown_receive);
			ssl_socket_.lowest_layer().close();
		}
		else {
			socket_.shutdown(tcp::socket::shutdown_receive);
			socket_.close();
		}

		return res;
	//}
	//catch (std::exception& e) {
	//	std::cerr << "Exception: " << e.what() << "\n";
	//}
}

//TODO: Actually verify, this is important!
bool http_client::verify_certificate(bool preverified, boost::asio::ssl::verify_context& ctx)
{
	return true;
}


void http_client::handle_parse_error(const char * what)
{
	std::cerr << "Parser Error: " << what << "\n";
}

int http_client::on_url(const char * buf, size_t len)
{
	std::cout << "Got url:" << std::string(buf, len) << "\n";
	return 0;
}

int http_client::on_header_field(const char * buf, size_t len)
{
	if (hps_ == HeaderParserState::HeaderStart) {
		//first call
		header_key_.append(buf, len);
		hps_ = HeaderParserState::HeaderValue;
	}
	//last state was a value so a complete kv is done.
	else if (hps_ == HeaderParserState::HeaderKey) {
		//headers_.insert(std::pair<std::string, std::string>(header_key_, header_value_));
		response_ptr_->add_header(std::move(header_key_), std::move(header_value_));
		header_key_.clear();
		header_value_.clear();
		header_key_.append(buf, len);
	}
	return 0; 
}

int http_client::on_header_value(const char * buf, size_t len)
{
	hps_ = HeaderParserState::HeaderKey;
	header_value_.append(buf, len);
	return 0;
}

int http_client::on_body(const char * buf, size_t len)
{
	std::string str(buf, len);
	response_ptr_->set_content(std::move(str));
	return 0;
}

int http_client::on_message_begin()
{
	hps_ = HeaderParserState::HeaderStart;
	return 0;
}

int http_client::on_header_complete()
{
	//push last kv pair to the headers
	//headers_.insert(std::pair<std::string, std::string>(header_key_, header_value_));
	response_ptr_->add_header(std::move(header_key_), std::move(header_value_));
	hps_ = HeaderParserState::HeadersComplete;
	return 0;
}

int http_client::on_url_cb(http_parser * parser, const char * buf, size_t len)
{
	http_client* cl = static_cast<http_client*>(parser->data);
	try {
		return cl->on_url(buf, len);
	}
	catch (const std::exception& e) {
		cl->handle_parse_error(e.what());
		return 1;
	}
}

int http_client::on_header_field_cb(http_parser * parser, const char * buf, size_t len)
{
	http_client* cl = static_cast<http_client*>(parser->data);
	try {
		return cl->on_header_field(buf, len);
	}
	catch (const std::exception& e) {
		cl->handle_parse_error(e.what());
		return 1;
	}
}

int http_client::on_header_value_cb(http_parser * parser, const char * buf, size_t len)
{
	http_client* cl = static_cast<http_client*>(parser->data);
	try {
		return cl->on_header_value(buf, len);
	}
	catch (const std::exception& e) {
		cl->handle_parse_error(e.what());
		return 1;
	}
}

int http_client::on_body_cb(http_parser * parser, const char * buf, size_t len)
{
	http_client* cl = static_cast<http_client*>(parser->data);
	try {
		return cl->on_body(buf, len);
	}
	catch (const std::exception& e) {
		cl->handle_parse_error(e.what());
		return 1;
	}
}

int http_client::on_message_begin_cb(http_parser* parser)
{
	http_client* cl = static_cast<http_client*>(parser->data);
	try {
		return cl->on_message_begin();
	}
	catch (const std::exception& e) {
		cl->handle_parse_error(e.what());
		return 1;
	}
}

int http_client::on_header_complete_cb(http_parser * parser)
{
	http_client* cl = static_cast<http_client*>(parser->data);
	try {
		return cl->on_header_complete();
	}
	catch (const std::exception& e) {
		cl->handle_parse_error(e.what());
		return 1;
	}
}
