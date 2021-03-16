#include "config_parser.h"
#include "make_response.h"
#include "not_found_handler.h"
#include "gtest/gtest.h"

class not_found_handler_test : public ::testing::Test {
protected:
    NginxConfig config1_;

    void SetUp() override {
    }
};

TEST_F(not_found_handler_test, NotFoundInitializeTest) {
    not_found_handler *handler = not_found_handler::Init("/", config1_);
    EXPECT_NE(handler, nullptr);
    delete handler;
}


TEST_F(not_found_handler_test, NotFoundHandleTest) {
    not_found_handler *handler = not_found_handler::Init("/", config1_);
    request req;
    response resp = handler->do_handle(req);
    response resp_exp = response::status_to_response(response::not_found);
    EXPECT_EQ(resp.data, resp_exp.data);
    delete handler;
}