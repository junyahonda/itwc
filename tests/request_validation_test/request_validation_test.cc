#include <string>
#include <array>
#include <fstream>
#include <streambuf>
#include "request_validation.h"
#include "data_structures.h"
#include "gtest/gtest.h"



void padIt(std::array<char, 2048> &arr, std::size_t& bytes_transferred, std::string origin) {    
    int j = 0;
    for (int i = 0; i < origin.size(); ++i) {
        if (origin[i] == '\n' && (i == 0 || origin[i-1] != '\r')) {
            arr[j++] = '\r';
            ++bytes_transferred;
        }
        arr[j++] = origin[i];
        ++bytes_transferred;

    }
}

class request_validation_test : public ::testing::Test {
protected:
    void SetUp() override {
        SetTest(test1_, req1_, req_val_1, "test_request_01");
        SetTest(test2_, req2_, req_val_2, "test_request_02");
        SetTest(test3_, req3_, req_val_3, "test_request_03");
    }
    void SetTest(bool &test, request &req, request_validator &req_val, const char *filename) {
        std::ifstream t(filename);
        std::string str((std::istreambuf_iterator<char>(t)),
                 std::istreambuf_iterator<char>());
        std::array<char, 2048> new_arr;
        std::size_t bytes_transferred = 0;
        padIt(new_arr, bytes_transferred, str);
        request_validator::request_result result;
        std::tie(result, std::ignore) = req_val.parse(
            req, new_arr.data(), new_arr.data() + bytes_transferred);
    }
    void ExtTest(request &req, request_validator &req_val, std::string &str) {
        std::array<char, 2048> new_arr;
        std::size_t bytes_transferred = 0;
        padIt(new_arr, bytes_transferred, str);
        request_validator::request_result result;
        std::tie(result, std::ignore) = req_val.parse(
            req, new_arr.data(), new_arr.data() + bytes_transferred);
    }

    bool test1_;
    bool test2_;
    bool test3_;

    request req1_;
    request req2_;
    request req3_;

    request_validator req_val_1;
    request_validator req_val_2;
    request_validator req_val_3;
};


TEST_F(request_validation_test, request_test_01) {
    std::string method = "GET";
    EXPECT_EQ(req1_.method, method);

}

TEST_F(request_validation_test, request_test_02) {
    std::string method = "POST";
    EXPECT_EQ(req2_.method, method);
}

TEST_F(request_validation_test, InvalidRequestTest) {
    request req;
    request_validator req_val;
    std::string req_str;
    req_str = "@@\r\n";
    ExtTest(req, req_val, req_str);
    req_val.reset();
    req_str = "P@\r\n";
    ExtTest(req, req_val, req_str);
    req_val.reset();
    req_str = "PUT @\r\n";
    ExtTest(req, req_val, req_str);
    req_val.reset();
    req_str = "PUT /something @\r\n";
    ExtTest(req, req_val, req_str);
    req_val.reset();
    req_str = "PUT /something H@\r\n";
    ExtTest(req, req_val, req_str);
    req_val.reset();
    req_str = "PUT /something HT@\r\n";
    ExtTest(req, req_val, req_str);
    req_val.reset();
    req_str = "PUT /something HTT@\r\n";
    ExtTest(req, req_val, req_str);
    req_val.reset();
    req_str = "PUT /something HTTP@\r\n";
    ExtTest(req, req_val, req_str);
    req_val.reset();
    req_str = "PUT /something HTTP/@\r\n";
    ExtTest(req, req_val, req_str);
    req_val.reset();
    req_str = "PUT /something HTTP/1@\r\n";
    ExtTest(req, req_val, req_str);
    req_val.reset();
    req_str = "PUT /something HTTP/11@\r\n";
    ExtTest(req, req_val, req_str);
    req_val.reset();
    req_str = "PUT /something HTTP/1.@\r\n";
    ExtTest(req, req_val, req_str);
    req_val.reset();
    req_str = "PUT /something HTTP/1.0@\r\n";
    ExtTest(req, req_val, req_str);
    req_val.reset();
    req_str = "PUT /something HTTP/1.00\r@";
    ExtTest(req, req_val, req_str);
    req_val.reset();
    req_str = "PUT /something HTTP/1.00@\r\n";
    ExtTest(req, req_val, req_str);
    req_val.reset();
    req_str = "PUT /something HTTP/1.0\r\n" +
    req_str+= "@";
    ExtTest(req, req_val, req_str);
    req_val.reset();
    req_str = "PUT /something HTTP/1.0\r\n  @";
    ExtTest(req, req_val, req_str);
    req_val.reset();
    req_str = "PUT /something HTTP/1.0\r\n  \r\n@";
    ExtTest(req, req_val, req_str);
    req_val.reset();
    req_str = "PUT /something HTTP/1.0\r\n  ";
    req_str += static_cast<char>(20);
    ExtTest(req, req_val, req_str);
    req_val.reset();
    req_str = "PUT /something HTTP/1.0\r\nHeader:NOSPACE\r\n@";
    ExtTest(req, req_val, req_str);
    req_val.reset();
    req_str = "PUT /something HTTP/1.0\r\nHeader: ";
    req_str += static_cast<char>(20);
    ExtTest(req, req_val, req_str);
    req_val.reset();
    req_str = "PUT /something HTTP/1.0\r\nHeader: value\r@";
    ExtTest(req, req_val, req_str);
    req_val.reset();
    req_str = "PUT /something HTTP/1.0\r\n\r@";
    ExtTest(req, req_val, req_str);
    req_val.reset();
}