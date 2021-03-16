#include "data_structures.h"
#include "config_parser.h"
#include "make_response.h"
#include "health_request_handler.h"
#include "gtest/gtest.h"

class health_handler_tester : public ::testing::Test {
protected:
    NginxConfig config_;
    health_request_handler* handler_;

    void SetUp() override{
        handler_ = health_request_handler::Init("/health", config_);
    }
};

TEST_F(health_handler_tester, health_return_test){
    request any_req;
    response resp = handler_->do_handle(any_req);

    EXPECT_EQ(resp.status, response::ok);
    EXPECT_EQ(resp.data, "OK");
    EXPECT_EQ(resp.header_map["Content-Type"], "text/plain");
    EXPECT_EQ(resp.header_map["Content-Length"], "2");
}