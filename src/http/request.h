#pragma once
#include "common.h"

#include "url.hpp"
#include "http_parser.h"

namespace http {
	class Request {
		friend class http_client;
	public:
		Request();
		~Request();

		//Url* get_url();
		//http_parser* get_parser();
		const std::string& get_user_agent() const;
		int get_retries() const;
		int get_connect_timeout() const;
		int get_read_timeout();

		//void set_url(Url& url);
		void set_retries(int retries);
		void set_connect_timeout(int timeout);
		void set_read_timeout(int timeout);

		Url& get_url()
		{
			return url_;
		}

		//void set_url(Url&& url);
		void set_url(Url url);
	private:
		std::string user_agent_;
		int connect_timeout_;
		int read_timeout_;
		int retries_;
		Url url_;
	};
}