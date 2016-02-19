//
// Created by dan on 1/17/16.
//
#pragma once

#include "gtest/gtest.h"
#include "http/client.h"

using namespace http;

//The fixture for the podbot's http_client class.
class HTTPTest : public ::testing::Test {
public:
    HTTPTest();
    virtual ~HTTPTest();

    boost::asio::io_service io_;
    HttpClient cl_;
};