#pragma once

#include "common.h"
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/bind.hpp>
#include "url.hpp"
#include <array>
#include <istream>
#include <ostream>
#include "http_parser.h"
#include "request.h"
#include "response.h"
#include <map>

namespace {
	enum class HeaderParserState {
		HeaderStart,
		HeaderKey,
		HeaderValue,
		HeadersComplete
	};
}


namespace http {
	class http_client {
	public:
		http_client();

		std::unique_ptr<Response> Req(Request* req);

	private:
		bool verify_certificate(bool preverified, boost::asio::ssl::verify_context& ctx);
		void handle_parse_error(const char* what);

		//http_parser callbacks
		int on_url(const char* buf, size_t len);
		int on_header_field(const char* buf, size_t len);
		int on_header_value(const char* buf, size_t len);
		int on_body(const char* buf, size_t len);
		int on_message_begin();
		int on_header_complete();

		//C wrappers for the callbacks
		static int on_url_cb(http_parser* parser, const char* buf, size_t len);
		static int on_header_field_cb(http_parser* parser, const char* buf, size_t len);
		static int on_header_value_cb(http_parser* parser, const char* buf, size_t len);
		static int on_body_cb(http_parser* parser, const char* buf, size_t len);
		static int on_message_begin_cb(http_parser* parser);
		static int on_header_complete_cb(http_parser* parser);

		HeaderParserState hps_;

		boost::asio::io_service service_;
		boost::asio::ssl::context ssl_ctx_;
		boost::asio::ip::tcp::resolver resolver_;
		boost::asio::ip::tcp::socket socket_;
		boost::asio::ssl::stream<boost::asio::ip::tcp::socket> ssl_socket_;

		boost::asio::streambuf request_;
		boost::asio::streambuf response_;

		std::string header_key_;
		std::string header_value_;

		http_parser parser_;
		Response* response_ptr_;
	};
}