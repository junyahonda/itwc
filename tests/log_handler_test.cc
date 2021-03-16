#include "gtest/gtest.h"
#include "log_handler.h"

TEST(setup_log_test, logHandlerTest) {
    EXPECT_TRUE(setup_log());
}

TEST(init_log_test, logHandlerTest) {
    EXPECT_TRUE(init_log());
}

TEST(simple_log_test, logHandlerTest) {
    std::string message = "test log message";
    for (int i = 0; i < 6; i++) {
        EXPECT_TRUE(handle_log(i, message));
    }

    //negative numbers
    int bad_num = -1;
    EXPECT_FALSE(handle_log(bad_num, message));

    //higher than 5
    bad_num = 6;
    EXPECT_FALSE(handle_log(bad_num, message));
}

TEST(ip_log_test, logHandlerTest) {
    std::string message = "test log message";
    std::string ip = "127.0.0.1";

    log_data tag;
    for (int i = 0; i < 6; i++) {
        EXPECT_TRUE(handle_machine_log(tag, i, message));
    }

    //negative numbers
    // int bad_num = -1;
    EXPECT_FALSE(handle_machine_log(tag, -1, message));
}