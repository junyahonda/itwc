#pragma once

#include "data_structures.h"
#include "make_response.h"

#include <string>

class response_parser {
    public:
    bool parse(std::string input_response, response& rsp, std::string& excess);

    void find_headers(const std::string& header_strings, response& rsp);

    bool validate_status_code(const std::string& status_line, response& rsp);
    bool validate_http_version(const std::string& status_line, response& rsp);
    bool validate_headers(const response& rsp);
    bool is_token(const char& c);
};
