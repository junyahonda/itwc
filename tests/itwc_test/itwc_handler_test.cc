#include "config_parser.h"
#include "make_response.h"
#include "itwc_handler.h"
#include "gtest/gtest.h"

class itwc_handler_test : public ::testing::Test {
protected:
    NginxConfig config1_;
    NginxConfig config2_;

    void SetUp() override {
        std::shared_ptr<NginxConfigStatement> root = std::shared_ptr<NginxConfigStatement>(new NginxConfigStatement);
        root->tokens_.push_back("root");
        root->tokens_.push_back("\"./\"");
        config1_.statements_.push_back(root);
    }
};

TEST_F(itwc_handler_test, itwcInitializeTest) {
    itwc_handler *handler = itwc_handler::Init("/itwc", config1_);
    EXPECT_NE(handler, nullptr);
    delete handler;
}

TEST_F(itwc_handler_test, itwcSimpleRequestTest) {
    itwc_handler *handler = itwc_handler::Init("/itwc", config1_);
    request req;
    req.method = "GET";
    req.uri = "/itwc/";
    req.http_version_major = 1;
    req.http_version_minor = 0;
    req.raw = "GET /itwc/ HTTP/1.0";

    response resp = handler->do_handle(req);
    EXPECT_EQ(resp.status, response::ok);

    delete handler;
}

TEST_F(itwc_handler_test, itwcSimpleBadRequestTest) {
    itwc_handler *handler = itwc_handler::Init("/itwc", config1_);
    request req;
    req.method = "POST";
    req.uri = "fadsjklasdn";
    req.http_version_major = 1;
    req.http_version_minor = 0;
    req.raw = "GET /itwc/dfb  asl HTTP/1.0";

    request req1;
    req1.method = "GET";
    req1.uri = "trash..///kjikml";
    req1.http_version_major = 1;
    req1.http_version_minor = 0;
    req1.raw = "GET /itwc/fadsblk HTTP/1.0";

    request req2;
    req2.method = "GET";
    req2.uri = "/itwc/bad.html";
    req2.http_version_major = 1;
    req2.http_version_minor = 0;
    req2.raw = "GET /itwc/bad.html HTTP/1.0";

    request req3;
    req1.method = "GET";
    req1.uri = "/itwc/%3/BAD";
    req1.http_version_major = 1;
    req1.http_version_minor = 0;
    req1.raw = "GET /itwc/fadsblk HTTP/1.0";

    response resp = handler->do_handle(req);
    response resp1 = handler->do_handle(req1);
    response resp2 = handler->do_handle(req2);
    response resp3 = handler->do_handle(req3);
    EXPECT_NE(resp.status, response::ok);
    EXPECT_NE(resp1.status, response::ok);
    EXPECT_NE(resp2.status, response::ok);
    EXPECT_NE(resp3.status, response::ok);
    delete handler;
}

TEST_F(itwc_handler_test, itwcAddingWinnerTest) {
    itwc_handler *handler = itwc_handler::Init("/itwc", config1_);
    request req;
    req.method = "GET";
    req.uri = "/itwc/winner/test_set/test1.jpg";
    req.http_version_major = 1;
    req.http_version_minor = 0;
    req.raw = "GET /itwc/ HTTP/1.0";

    response resp = handler->do_handle(req);
    EXPECT_EQ(resp.status, response::ok);

    delete handler;
}
