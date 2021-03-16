#include "config_parser.h"
#include "make_response.h"
#include "echo_request_handler.h"
#include "gtest/gtest.h"

class echo_handler_test : public ::testing::Test {
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

TEST_F(echo_handler_test, EchoInitializeTest) {
    echo_request_handler *handler = echo_request_handler::Init("/echo", config1_);
    EXPECT_NE(handler, nullptr);
    delete handler;
}

TEST_F(echo_handler_test, EchoSimpleRequestTest) {
    echo_request_handler *handler = echo_request_handler::Init("/echo", config1_);
    request req;
    req.method = "GET";
    req.uri = "/echo/sample.txt";
    req.http_version_major = 1;
    req.http_version_minor = 0;
    req.raw = "GET /echo/sample.txt HTTP/1.0";

    response resp = handler->do_handle(req);
    EXPECT_EQ(resp.status, response::ok);
    EXPECT_EQ(resp.data, req.raw);
    delete handler;
}

TEST_F(echo_handler_test, EchoInvalidRequestTest) {
    echo_request_handler *handler = echo_request_handler::Init("/echo", config1_);
    request req;
    req.method = "GET";
    req.uri = "/echo/%W%RongFile.txt";
    req.http_version_major = 1;
    req.http_version_minor = 0;
    req.raw = "GET /echo/%W%RongFile.txt HTTP/1.0";

    response resp = handler->do_handle(req);
    response expected_resp = response::status_to_response(response::bad_request);
    EXPECT_EQ(resp.status, response::bad_request);
    EXPECT_EQ(resp.data, expected_resp.data);
    delete handler;
}

TEST_F(echo_handler_test, EchoAppendIndexTest) {
    echo_request_handler *handler = echo_request_handler::Init("/echo", config1_);
    request req;
    req.method = "GET";
    req.uri = "/echo/";
    req.http_version_major = 1;
    req.http_version_minor = 0;
    req.raw = "GET /echo/ HTTP/1.0";

    response resp = handler->do_handle(req);
    EXPECT_EQ(resp.status, response::ok);
    EXPECT_EQ(resp.data, req.raw);
    delete handler;
}

TEST_F(echo_handler_test, EchoBadRequestTest) {
    echo_request_handler *handler = echo_request_handler::Init("/echo", config1_);
    request req;
    req.method = "GET";
    req.uri = "wrong_uri/";
    req.http_version_major = 1;
    req.http_version_minor = 0;
    req.raw = "GET /echowrong_uri/ HTTP/1.0";

    response resp = handler->do_handle(req);
    response expected_resp = response::status_to_response(response::bad_request);

    EXPECT_EQ(resp.status, response::bad_request);
    EXPECT_EQ(resp.data, expected_resp.data);
    delete handler;
}

TEST_F(echo_handler_test, EchoWrongFileTest) {
    echo_request_handler *handler = echo_request_handler::Init("/echo", config1_);
    request req;
    req.method = "GET";
    req.uri = "/wrong_file.txt";
    req.http_version_major = 1;
    req.http_version_minor = 0;
    req.raw = "GET /echo/wrong_file.txt HTTP/1.0";

    response resp = handler->do_handle(req);
    response expected_resp = response::status_to_response_echo(response::not_found, req);

    EXPECT_EQ(resp.status, response::not_found);
    EXPECT_EQ(resp.data, expected_resp.data);
    delete handler;
}