#include "config_parser.h"
#include "data_structures.h"
#include "gtest/gtest.h"

class data_structures_test : public ::testing::Test {
protected:
    header header_;
    request request_;
    handler_config handler_config_;

    void SetUp() override{
        header_.name = "DUMMY";
        header_.value = "VALUE";

        request_.method = "1";
        request_.uri    = "2";
        request_.http_version_major = 3;
        request_.http_version_minor = 4;
        request_.headers.push_back(header_);
        request_.headers_map[header_.name] = header_.value;
        request_.path   = "5";
        request_.raw    = "6";
        request_.ip     = "7";

        handler_config_.type = handler_config::Not_found;
        handler_config_.location_prefix = "8";
    }
};

TEST_F(data_structures_test, headerAssignmentTest) {
    EXPECT_EQ(header_.name, "DUMMY");
    EXPECT_EQ(header_.value, "VALUE");
}

TEST_F(data_structures_test, requestAssignmentTest) {
    EXPECT_EQ(request_.method, "1");
    EXPECT_EQ(request_.uri, "2");
    EXPECT_EQ(request_.http_version_major, 3);
    EXPECT_EQ(request_.http_version_minor, 4);
    EXPECT_EQ((request_.headers.begin())->name, header_.name);
    EXPECT_EQ((request_.headers.begin())->value, header_.value);
    EXPECT_EQ(request_.headers_map["DUMMY"], "VALUE");
    EXPECT_EQ(request_.path, "5");
    EXPECT_EQ(request_.raw, "6");
    EXPECT_EQ(request_.ip, "7");
}

TEST_F(data_structures_test, handlerConfigAssignmentTest) {
    EXPECT_EQ(handler_config_.type, handler_config::Not_found);
    EXPECT_EQ(handler_config_.location_prefix, "8");
}
