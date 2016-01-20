//
// Created by dan on 1/20/16.
//

#pragma once

#include "gtest/gtest.h"
#include "common/date_time.h"
#include "common/config.h"

//The fixture for testing the common features of podbot
//e.g. ConfigMgr, JSON, XML, Log and DateTime
class CommonTest : public ::testing::Test {
public:
    CommonTest();
    virtual ~CommonTest();

    void SetUp();
    void TearDown();

    ConfigMgr* config_mgr_;
    DateTime* date_time_;
};