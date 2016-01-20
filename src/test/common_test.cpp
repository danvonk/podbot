//
// Created by dan on 1/20/16.
//

#include "common_test.h"

CommonTest::CommonTest() {

}

CommonTest::~CommonTest() {

}

void CommonTest::SetUp() {
    config_mgr_ = new ConfigMgr();
    date_time_ = new DateTime();
}

void CommonTest::TearDown() {
    delete config_mgr_;
    delete date_time_;
}

TEST_F(CommonTest, GetConfigVariables) {
    config_mgr_->ParseConfigFile("../res/podbot.cfg");
    //These variables should never be empty as they are given default values in that case.
    EXPECT_EQ(false, config_mgr_->GetValue<std::string>("db_user", "root").empty());
    EXPECT_EQ(false, config_mgr_->GetValue<std::string>("db_pass", "user").empty());
    EXPECT_EQ(false, config_mgr_->GetValue<std::string>("db_name", "cpp").empty());
    EXPECT_EQ(false, config_mgr_->GetValue<std::string>("db_host", "localhost").empty());
    EXPECT_EQ(3306, config_mgr_->GetValue<int>("db_port", 3306));
    EXPECT_EQ(30, config_mgr_->GetValue<int>("db_ping_time", 30));
}

TEST_F(CommonTest, ParseRFC2822Date) {
    date_time_->ParseRFC2822("Tue, 19 Jan 2016 11:32:57 -0700");
    EXPECT_EQ(19, date_time_->Day());
    EXPECT_EQ(1, date_time_->Month());
    EXPECT_EQ(2016, date_time_->Year());
    EXPECT_EQ(11, date_time_->Hour());
    EXPECT_EQ(32, date_time_->Minute());
    EXPECT_EQ(57, date_time_->Second());
}