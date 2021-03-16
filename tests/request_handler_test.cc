#include "data_structures.h" // request
#include "make_response.h" // response
#include "base_request_handler.h" // base_request_handler
#include "echo_request_handler.h" // echo_request_handler
#include "static_request_handler.h" // static_request_handler
#include "log_handler.h"
#include "gtest/gtest.h"
#include <string>


// TODO(cpp)
// fix the segfault issue with this unit test

class request_handler_test : public ::testing::Test {
    enum handler_type {
        BASE_HANDLER,
        ECHO_HANDLER,
        STATIC_HANDLER
    } t;
protected:
    void SetUp() override {
        std::string base_path = "./test_data";
        std::string echo_path = "./test_data/echo";
        std::string static_path = "./test_data/static";

        SetTest(base_, base_path, BASE_HANDLER);
        SetTest(echo_, echo_path, ECHO_HANDLER);
        SetTest(static_, static_path, STATIC_HANDLER);

        uriTest();
    }
    
    void SetTest(base_request_handler *handler, std::string &root_path, handler_type type) {
        switch (type) {
            case BASE_HANDLER: handler = new base_request_handler(root_path); break;
            case ECHO_HANDLER: handler = new echo_request_handler(root_path); break;
            case STATIC_HANDLER: handler = new static_request_handler(root_path); break;
            default: break;
        }
        
    }

    void uriTest() {
        request new_req;
        header head;
        new_req.method = "GET";
        // new_req.uri = "%%2Ftesting%%2Fdirectories%%2Fhello.jpg";
        new_req.uri = "/hello.jpg";
        new_req.http_version_major = 1;
        new_req.http_version_minor = 1;
        head.name = "Host";
        head.value = "1.1.1.1";
        new_req.headers.push_back(head);
        new_req.path = "Non-existing path";
        new_req.raw = "This is raw request data";
        new_req.ip = "client ip";
        request_ = new_req;
        echo_->do_handle(request_, response_);
    }

    void setRequest(request &req) {
        request_ = req;
    }

    void callHandle(base_request_handler *handler) {
        handler->do_handle(request_, response_);
    }

    request request_;
    response response_;

    base_request_handler *base_;
    echo_request_handler *echo_;
    static_request_handler *static_;
};


TEST_F(request_handler_test, request_test_convert_hex) {
    // request new_req;
    // header head;
    // new_req.method = "GET";
    // new_req.uri = "%%2Ftesting%%2Fdirectories%%2Fhello.jpg";
    // new_req.uri = "/hello.jpg";
    // new_req.http_version_major = 1;
    // new_req.http_version_minor = 1;
    // head.name = "Host";
    // head.value = "1.1.1.1";
    // new_req.path = "Non-existing path";
    // new_req.raw = "This is raw request data";
    // new_req.ip = "client ip";

    // setRequest(new_req);
    // callHandle(base_);

    EXPECT_NE(request_.uri, "");
}