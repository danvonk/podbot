//
// Created by dan on 1/17/16.
//

#include "http_test.h"


HTTPTest::HTTPTest()
    : cl_(io_)
{
}

HTTPTest::~HTTPTest() {
}

TEST_F(HTTPTest, HTTPSGetRequest) {
    Request req;
    req.set_url(Url("https://danvonk.com"));
    auto res = cl_.Req(&req);

    EXPECT_EQ(200, res.get_status_code());
    EXPECT_EQ(false, res.get_content().empty());
}

TEST_F(HTTPTest, ItunesPageIteration) {
    for (int i = 0; i < 27; ++i) {
        Url url("https://example.com");
        url.add_query("mt", "2");
        url.add_query("letter");
        url.add_query("page");

        if (i == 26) {
            url.set_query(1).val("*");
        } else {
            url.set_query(1).val(std::string(1, static_cast<char>(i + 65)));
        }

        int page = 1;
        while (true) {
            url.set_query(2).val(std::to_string(page));

            if (page > 3) {
                break;
            }

            std::stringstream ss;
            ss << "https://example.com?mt=2";
            if (i == 26) {
                ss << "&letter=" << "*";
            } else {
                ss << "&letter=" << static_cast<char>(i + 65);
            }
            ss << "&page=" << page;

            EXPECT_EQ(ss.str(), url.str());
            page++;
        }
    }
}