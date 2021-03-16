#include "gtest/gtest.h"
#include "handle_dispatch.h"
#include "data_structures.h"
#include "echo_request_handler.h"
#include "static_request_handler.h"
#include "make_response.h"
#include "config_parser.h"
#include "log_handler.h"
#include <vector>

class handle_dispatch_test : public ::testing::Test {
protected:
	void setup(std::map<std::string, base_request_handler*> mp_handler) {
		dispath_ = new handle_dispatch(mp_handler);
	}
	void deconst() {
		delete dispath_;
	}
	handle_dispatch* dispath_;
};

TEST_F(handle_dispatch_test, handle_dispatch_test_1) {
	NginxConfig c1;
	NginxConfigParser parser;

	const char* in_config = "test_dispatch_config_1";
	parser.Parse(in_config, &c1);

	std::map<std::string, base_request_handler*> temp_mp;
	temp_mp["/data1"] = echo_request_handler::Init("/data1", c1);
	setup(temp_mp);
	std::string raw_data = "GET /data1/index.html HTTP/1.1\n\n";
	request req;
	req.raw = raw_data;
	req.uri = "/data1/index.html";
	response resp = dispath_->match_handler(req);

    EXPECT_EQ(resp.status, response::ok);
    // EXPECT_EQ(resp.data, raw_data);
    // EXPECT_EQ(resp.header_map["Content-Length"], std::to_string(resp.data.size()));
    // EXPECT_EQ(resp.header_map["Content-Type"], "text/plain");
    deconst();
}

// TEST_F(handle_dispatch_test, handle_dispatch_test_2) {
// 	std::string raw_data = "GET /data2/www/index.html HTTP/1.1\n\n";
// 	request req;
// 	req.raw = raw_data;
// 	req.uri = "/data2/www/index.html";

// 	response resp = dispath_->match_handler(req);


// }