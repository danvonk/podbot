#include "client.h"
#include <boost/asio/use_future.hpp>
#include "http_exception.h"

using namespace http;
namespace asio = boost::asio;
using asio::ip::tcp;

http_parser_settings parser_settings_;

HttpClient::HttpClient(asio::io_service &io)
	: service_(io), hps_(HeaderParserState::HeaderStart)
	, ssl_ctx_(asio::ssl::context::sslv23)
	, resolver_(service_)
	, using_ssl_(false)
	, socket_(service_)
	, ssl_socket_(service_, ssl_ctx_)
{
	ssl_ctx_.set_default_verify_paths();
	ssl_ctx_.load_verify_file("../res/cacert.pem");
	ssl_ctx_.set_verify_mode(asio::ssl::verify_peer);

	parser_settings_.on_url = HttpClient::on_url_cb;
	parser_settings_.on_header_field = HttpClient::on_header_field_cb;
	parser_settings_.on_header_value = HttpClient::on_header_value_cb;
	parser_settings_.on_body = HttpClient::on_body_cb;
	parser_settings_.on_headers_complete = HttpClient::on_header_complete_cb;
}

Response HttpClient::make_request(Request *req) {
	bool usingSSL = false;
	Response res;

	res.status_code_ = 0;
	res.content_ = "";

	//initialise parser
	http_parser parser;
	http_parser_init(&parser, HTTP_RESPONSE);
	parser.data = this;
	response_ptr_ = &res;

	if (req->get_url().scheme() == "https") {
		usingSSL = true;
	}

	asio::streambuf request;
	std::ostream req_str(&request);
	req_str << "GET " << req->get_url().pathForRequest() << " HTTP/1.0\r\n";
	req_str << "Host: " << req->get_url().host() << "\r\n";
	req_str << "Accept: */*\r\n";
	req_str << "Connection: close\r\n\r\n";

	tcp::resolver::query query(req->get_url().host(), req->get_url().scheme());

	if (usingSSL) {
		try {
			asio::connect(ssl_socket_.lowest_layer(), resolver_.resolve(query));
			ssl_socket_.lowest_layer().set_option(tcp::no_delay(true));
			ssl_socket_.set_verify_mode(asio::ssl::verify_peer);
			ssl_socket_.handshake(asio::ssl::stream_base::client);
			asio::write(ssl_socket_, request);

			//read until eof
			boost::system::error_code error;
			asio::streambuf response;
			while (asio::read(ssl_socket_, response, asio::transfer_at_least(64), error)) {
				size_t bytesParsed = http_parser_execute(&parser, &parser_settings_, asio::buffer_cast<const char*>(response.data()), response.size() * sizeof(char));
				response.consume(response.size());
			}
			
			if (error == asio::error::eof) {
				if (http_parser_execute(&parser, &parser_settings_, nullptr, 0) != 0) {
					throw HTTPException("Error parsing at HTTP at EOF.");
				}
			}
			else {
				PBLOG_ERROR << "HTTP(S) ASIO Error: " << error.message();
			}
		}
		catch (const std::exception& e) {
			PBLOG_ERROR << "HTTP(S) Error: " << e.what();
			return res;
		}

		ssl_socket_.lowest_layer().shutdown(tcp::socket::shutdown_receive);
		ssl_socket_.lowest_layer().close();
	} else {
		asio::connect(socket_, resolver_.resolve(query));
		socket_.lowest_layer().set_option(tcp::no_delay(true));
		asio::write(socket_, request);

		//read until eof
		boost::system::error_code error;
		asio::streambuf response;
		while (asio::read(socket_, response, asio::transfer_at_least(64), error)) {
			size_t bytesParsed = http_parser_execute(&parser, &parser_settings_, asio::buffer_cast<const char*>(response.data()), response.size() * sizeof(char));
			response.consume(response.size());
		}

		if (error == asio::error::eof) {
			if (http_parser_execute(&parser, &parser_settings_, nullptr, 0) != 0) {
				throw HTTPException("Error parsing at HTTP at EOF.");
			}
		}
		else {
			PBLOG_ERROR << "HTTP(S) ASIO Error: " << error.message();
		}

		socket_.shutdown(tcp::socket::shutdown_receive);
		socket_.close();
	}

	//temporary sleep to stop being blocked by iTunes
	std::this_thread::sleep_for(std::chrono::seconds(5));
	return res;
}


void HttpClient::handle_parse_error(const char *what) {
	PBLOG_ERROR << "HTTP Parse Error: " << what;
}

int HttpClient::on_url(const char *buf, size_t len) {
	std::cout << "Got url:" << std::string(buf, len) << "\n";
	return 0;
}

int HttpClient::on_header_field(const char *buf, size_t len) {
	if (hps_ == HeaderParserState::HeaderStart) {
		//first call
		header_key_.append(buf, len);
		hps_ = HeaderParserState::HeaderValue;
	}
	//last state was a value so a complete kv is done.
	else if (hps_ == HeaderParserState::HeaderKey) {
		//headers_.insert(std::pair<std::string, std::string>(header_key_, header_value_));
		response_ptr_->headers_.insert(std::pair<std::string, std::string>(std::move(header_key_), std::move(header_value_)));
		header_key_.clear();
		header_value_.clear();
		header_key_.append(buf, len);
	}
	return 0;
}

int HttpClient::on_header_value(const char *buf, size_t len) {
	hps_ = HeaderParserState::HeaderKey;
	header_value_.append(buf, len);
	return 0;
}

int HttpClient::on_body(const char *buf, size_t len) {
	response_ptr_->content_.append(buf, len);
	return 0;
}

int HttpClient::on_message_begin() {
	hps_ = HeaderParserState::HeaderStart;
	return 0;
}

int HttpClient::on_header_complete() {
	//push last kv pair to the headers
	//headers_.insert(std::pair<std::string, std::string>(header_key_, header_value_));
	response_ptr_->headers_.insert(std::pair<std::string, std::string>(std::move(header_key_), std::move(header_value_)));
	hps_ = HeaderParserState::HeadersComplete;
	return 0;
}

int HttpClient::on_url_cb(http_parser *parser, const char *buf, size_t len) {
	HttpClient *cl = static_cast<HttpClient *>(parser->data);
	try {
		return cl->on_url(buf, len);
	}
	catch (const std::exception &e) {
		cl->handle_parse_error(e.what());
		return 1;
	}
}

int HttpClient::on_header_field_cb(http_parser *parser, const char *buf, size_t len) {
	HttpClient *cl = static_cast<HttpClient *>(parser->data);
	try {
		return cl->on_header_field(buf, len);
	}
	catch (const std::exception &e) {
		cl->handle_parse_error(e.what());
		return 1;
	}
}

int HttpClient::on_header_value_cb(http_parser *parser, const char *buf, size_t len) {
	HttpClient *cl = static_cast<HttpClient *>(parser->data);
	try {
		return cl->on_header_value(buf, len);
	}
	catch (const std::exception &e) {
		cl->handle_parse_error(e.what());
		return 1;
	}
}

int HttpClient::on_body_cb(http_parser *parser, const char *buf, size_t len) {
	HttpClient *cl = static_cast<HttpClient *>(parser->data);
	try {
		return cl->on_body(buf, len);
	}
	catch (const std::exception &e) {
		cl->handle_parse_error(e.what());
		return 1;
	}
}

int HttpClient::on_message_begin_cb(http_parser *parser) {
	HttpClient *cl = static_cast<HttpClient *>(parser->data);
	try {
		return cl->on_message_begin();
	}
	catch (const std::exception &e) {
		cl->handle_parse_error(e.what());
		return 1;
	}
}

int HttpClient::on_header_complete_cb(http_parser *parser) {
	HttpClient *cl = static_cast<HttpClient *>(parser->data);
	try {
		cl->response_ptr_->status_code_ = parser->status_code;
		return cl->on_header_complete();
	}
	catch (const std::exception &e) {
		cl->handle_parse_error(e.what());
		return 1;
	}
}
