#include "base_request_handler.h"
#include "data_structures.h"
#include "make_response.h"
#include "log_handler.h"
#include "gtest/gtest.h"

class make_response_test : public ::testing::Test {
protected:
    response resp_;
    request req_;
    void SetUp() override {
    }
};


TEST_F(make_response_test, makeResponseTest) {
    resp_ = response::status_to_response(response::ok);
    resp_ = response::status_to_response(response::created);
    resp_ = response::status_to_response(response::accepted);
    resp_ = response::status_to_response(response::no_content);
    resp_ = response::status_to_response(response::multiple_choices);
    resp_ = response::status_to_response(response::moved_permanently);
    resp_ = response::status_to_response(response::moved_temporarily);
    resp_ = response::status_to_response(response::not_modified);
    resp_ = response::status_to_response(response::bad_request);
    resp_ = response::status_to_response(response::unauthorized);
    resp_ = response::status_to_response(response::forbidden);
    resp_ = response::status_to_response(response::not_found);
    resp_ = response::status_to_response(response::internal_server_error);
    resp_ = response::status_to_response(response::not_implemented);
    resp_ = response::status_to_response(response::bad_gateway);
    resp_ = response::status_to_response(response::service_unavailable);
}

TEST_F(make_response_test, makeBufferTest) {
    std::vector<boost::asio::const_buffer> buffer;
    resp_.status = response::ok;
    buffer = resp_.to_buffers();
    resp_.status = response::created;
    buffer = resp_.to_buffers();
    resp_.status = response::accepted;
    buffer = resp_.to_buffers();
    resp_.status = response::no_content;
    buffer = resp_.to_buffers();
    resp_.status = response::multiple_choices;
    buffer = resp_.to_buffers();
    resp_.status = response::moved_permanently;
    buffer = resp_.to_buffers();
    resp_.status = response::moved_temporarily;
    buffer = resp_.to_buffers();
    resp_.status = response::not_modified;
    buffer = resp_.to_buffers();
    resp_.status = response::bad_request;
    buffer = resp_.to_buffers();
    resp_.status = response::unauthorized;
    buffer = resp_.to_buffers();
    resp_.status = response::forbidden;
    buffer = resp_.to_buffers();
    resp_.status = response::not_found;
    buffer = resp_.to_buffers();
    resp_.status = response::internal_server_error;
    buffer = resp_.to_buffers();
    resp_.status = response::not_implemented;
    buffer = resp_.to_buffers();
    resp_.status = response::bad_gateway;
    buffer = resp_.to_buffers();
    resp_.status = response::service_unavailable;
    buffer = resp_.to_buffers();

}
