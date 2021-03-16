#include "data_structures.h"
#include "make_response.h"
#include "response_parser.h"
#include <string>

bool response_parser::parse(std::string input_response, response& rsp, std::string& excess) {
    // Find end of headers
    size_t end_of_headers = input_response.find("\r\n\r\n");
    if (end_of_headers == std::string::npos) {
        // Incomplete response given
        return false;
    }

    std::string status_line = input_response.substr(0, input_response.find("\r\n"));
    std::string header_strings = input_response.substr(status_line.size());
    if (end_of_headers - status_line.size() > 0) {
        // There are headers
        header_strings = header_strings.substr(2, header_strings.find("\r\n\r\n"));
    }
    else {
        header_strings = "";
    }

    find_headers(header_strings, rsp);

    bool valid_response = true;
    valid_response &= validate_status_code(status_line, rsp);
    valid_response &= validate_http_version(status_line, rsp);
    valid_response &= validate_headers(rsp);

    if (rsp.header_map.find("Content-Length") == rsp.header_map.end()) {
        // No Content-Length but a body requires a 411 response, leave it
        // for the caller to check
        rsp.data = input_response.substr(end_of_headers + 4);
    }
    else {
        int body_size = std::stoi(rsp.header_map.at("Content-Length"));

        if (input_response.size() < end_of_headers + 4 + body_size) {
            // Incomplete
            valid_response &= false;
        }
        else {
            rsp.data = input_response.substr(end_of_headers + 4, body_size);
            valid_response &= rsp.data.size() == body_size;
            excess = input_response.substr(end_of_headers + 4 + body_size);
        }
    }

    return valid_response;
}

void response_parser::find_headers(const std::string& header_strings, response& rsp) {
    std::string remaining_headers = header_strings;
    while (!remaining_headers.empty()) {
        size_t end_of_header = remaining_headers.find("\r\n");
        std::string header_line = remaining_headers.substr(0, end_of_header);
        remaining_headers = (end_of_header == std::string::npos) ? "" : remaining_headers.substr(end_of_header + 2);
        
        size_t colon_pos = header_line.find(':');
        std::string header = header_line.substr(0, colon_pos);
        std::string value = header_line.substr(colon_pos + 1);

        size_t value_trim_front = 0;
        size_t value_trim_back = value.size() - 1;
        while (isspace(value[value_trim_front])) value_trim_front++;
        while (isspace(value[value_trim_back])) value_trim_back--;
        value = value.substr(value_trim_front, value_trim_back - value_trim_front + 1);

        rsp.header_map.insert({header, value});
    }
}

bool response_parser::validate_status_code(const std::string& status_line, response& rsp) {
    size_t start_of_status = 0;

    // Ignore leading white space.
    while (isspace(status_line[start_of_status])) {
        start_of_status++;
        if (start_of_status == status_line.size()) {
            return false;
        }
    }
    // Ignore the next word (should be the HTTP version)
    while (!isspace(status_line[start_of_status])) {
        start_of_status++;
        if (start_of_status == status_line.size()) {
            return false;
        }
    }

    // Ignore any white space between the previous word and
    // what should be the status
    while (isspace(status_line[start_of_status])) {
        start_of_status++;
        if (start_of_status == status_line.size()) {
            return false;
        }
    }

    // Find any following digits
    size_t end_of_status = start_of_status;
    while (isdigit(status_line[end_of_status])) {
        end_of_status++;
        if (end_of_status == status_line.size()) {
            // Number extends to the end of the status line
            // so there is no message, so the status line
            // is bad
            return false;
        }
    }
    if (end_of_status == start_of_status) {
        return false;
    }

    std::string status = status_line.substr(start_of_status, end_of_status - start_of_status);
    rsp.status = response::status_type(std::stoi(status));
    
    // Status codes are 3 digits and are in the range 100-599
    return status.size() == 3 && status[0] - '0' < 5 && status[0] != '0';
}

bool response_parser::validate_http_version(const std::string& status_line, response& rsp) {
    // Ignore trailing white space
    size_t start_of_version = 0;
    while (isspace(status_line[start_of_version])) {
        start_of_version++;
        if (start_of_version == status_line.size()) {
            return false;
        }
    }
    std::string version = status_line.substr(start_of_version, 8);
    return version.substr(0, 5) == "HTTP/" && isdigit(version[5]) && isdigit(version[7]);
}

bool response_parser::validate_headers(const response& rsp) {
    for (const auto& header_value : rsp.header_map) {
        const auto& header = header_value.first;
        const auto& value = header_value.second;


        for (const auto& c : header) {
            if (!is_token(c)) {
                return false;
            }
        }
        
        if (header == "Content-Length") {
            int length = std::stoi(value);
            if (std::to_string(length) != value || length < 0) {
                return false;
            }
        }
    }
    return true;
}

bool response_parser::is_token(const char& c)
{
  return static_cast<bool>(isalpha(c)) || static_cast<bool>(isdigit(c)) ||
         ('#' <= c && c <= '\'') || (c == '*') || (c == '+') || (c == '-') ||
         (c == '|');
}
