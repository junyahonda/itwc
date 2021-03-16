#include "request_validation.h"
#include "data_structures.h"

request_validator::request_validator()
                  :current_step_(start_init) {}


void request_validator::reset()
{ current_step_ = start_init; }


request_validator::request_result request_validator::validate(request &req, char input) {
    // operate depending on current step
    switch (current_step_) {

    // 1. initial state - expecting to have regular ccharacter
    //    no special nor control character
    case start_init:
        if (!is_ascii(input) || is_control(input) || is_special(input)) {
            return fail;
        } else {
            current_step_ = start;
            req.method.push_back(input);
            return in_progress;
        }

    // 2. start state - continue to push non-control, non-special
    //    chars to method name until a space is found. then turn
    //    the state to expecting uri
    case start:
        if (input == ' ') {
            current_step_ = get_uri;
            return in_progress;
        } else if (!is_ascii(input) || is_control(input) || is_special(input)) {
            return fail;
        } else {
            req.method.push_back(input);
            return in_progress;
        }

    // 3. get_uri state - continue to append uri chars that are not
    //    control char until we find a space char. Then expect HTTP
    case get_uri:
        if (input == ' ') {
            current_step_ = get_http_version_h;
            return in_progress;
        } else if (is_control(input)) {
            return fail;
        } else {
            req.uri.push_back(input);
            return in_progress;
        }

    // 4. get_http_version_h state - expect H from HTTP version
    //    i.e. HTTP/1.1
    case get_http_version_h:
        if (input == 'H') {
            current_step_ = get_http_version_t_1;
            return in_progress;
        } else {
            return fail;
        }

    // 5. get_http_version_t_1 state - expect the first T from
    //    HTTP version
    case get_http_version_t_1:
        if (input == 'T') {
            current_step_ = get_http_version_t_2;
            return in_progress;
        } else {
            return fail;
        }

    // 6. get_http_version_t_2 state - expect the second T from HTTP
    case get_http_version_t_2:
        if (input == 'T') {
            current_step_ = get_http_version_p;
            return in_progress;
        } else {
            return fail;
        }

    // 7. get_http_version_p state - expect the P from HTTP
    case get_http_version_p:
        if (input == 'P') {
            current_step_ = get_http_version_slash;
            return in_progress;
        } else {
            return fail;
        }

    // 8. get_http_version_slash - expect '/' from
    //    HTTP/1.1
    case get_http_version_slash:
        if (input == '/') {
            req.http_version_major = 0;
            req.http_version_minor = 0;
            current_step_ = get_http_version_major_start;
            return in_progress;
        } else {
            return fail;
        }

    // 9. get_http_version_major_start - expects a number (#)
    //    from HTTP/##.NN
    case get_http_version_major_start:
        if (is_num(input)) {
            req.http_version_major = req.http_version_major * 10 + (input - '0');
            current_step_ = get_http_version_major;
            return in_progress;
        } else {
            return fail;
        }

    // 10. get_http_version_major - continue to expect number (#)
    //     from HTTP/##.NN. if '.' is found, start parsing for
    //     minor digits
    case get_http_version_major:
        if (input == '.') {
            current_step_ = get_http_version_minor_start;
            return in_progress;
        } else if (is_num(input)) {
            req.http_version_major = req.http_version_major * 10 + (input - '0');
            return in_progress;
        } else {
            return fail;
        }

    // 11. get_http_version_minor_start - expect a number (N)
    //     from HTTP/##.NN
    case get_http_version_minor_start:
        if (is_num(input)) {
            req.http_version_minor = req.http_version_minor * 10 + (input - '0');
            current_step_ = get_http_version_minor;
            return in_progress;
        } else {
            return fail;
        }

    // 12. get_http_version_minor - continue to expect (N)
    //     from HTTP/##.NN. if a carriage return is found,
    //     start looking for a newline character
    case get_http_version_minor:
        if (input == '\r') {
            current_step_ = get_newline_before_header;
            return in_progress;
        } else if (is_num(input)) {
            req.http_version_minor = req.http_version_minor * 10 + (input - '0');
            return in_progress;
        } else {
            return fail;
        }

    // 13. get_newline_before_header - expect a newline
    //     then start parsing for header
    case get_newline_before_header:
        if (input == '\n') {
            current_step_ = get_header_start;
            return in_progress;
        } else {
            return fail;
        }

    // 14. get_header_start - start of the header
    //     if carriage return is found, expect newline char to end
    //     if it is a space, get rid of spaces and get value
    //     if they are control or special char, return fail
    //     otherwise create a new header with current character
    //     and add it to the request object and continue to parse name
    case get_header_start:
        if (input == '\r') {
            current_step_ = get_newline_end;
            return in_progress;
        } else if (!req.headers.empty() && (input == ' ' || input == '\t')) {
            current_step_ = get_header_space;
            return in_progress;
        } else if (!is_ascii(input) || is_control(input) || is_special(input)) {
            return fail;
        } else {
            req.headers.push_back(header());
            req.headers.back().name.push_back(input);
            current_step_ = get_header_name;
            return in_progress;
        }

    // 15. get_header_space - continue to skip spaces and
    //     if carriage is found, expect newline to end header parse
    //     if found valid char, consider it as a value and append it
    //     to current header
    case get_header_space:
        if (input == '\r') {
            current_step_ = get_newline_after_header;
            return in_progress;
        } else if (input == ' ' || input == '\t') {
            return in_progress;
        } else if (is_control(input)) {
            return fail;
        } else {
            req.headers.back().value.push_back(input);
            current_step_ = get_header_value;
            return in_progress;
        }

    // 16. get_header_name - continue to parse header name until
    //     ':' is found. then expect a space
    case get_header_name:
        if (input == ':') {
            current_step_ = get_space_after_header;
            return in_progress;
        } else if (!is_ascii(input) || is_control(input) || is_special(input)) {
            return fail;
        } else {
            req.headers.back().name.push_back(input);
            return in_progress;
        }

    // 17. get_space_after_header state - spect a space before value
    case get_space_after_header:
        if (input == ' ') {
            current_step_ = get_header_value;
            return in_progress;
        } else {
            return fail;
        }

    // 18. get_header_value - continue to parse value until \r is found
    //     then change to newline after the header step
    case get_header_value:
        if (input == '\r') {
            current_step_ = get_newline_after_header;
            return in_progress;
        } else if (is_control(input)) {
            return fail;
        } else {
            req.headers.back().value.push_back(input);
            return in_progress;
        }

    // 19. get_newline_after_header - expect a newline char
    //     and expect another header
    case get_newline_after_header:
        if (input == '\n') {
            current_step_ = get_header_start;
            return in_progress;
        } else {
            return fail;
        }

    // 20. get_newline_end - expect a newline. if so,
    //     parsing is completed.
    case get_newline_end:
        return (input == '\n') ? success : fail;

    // 21. default
    default:
        return fail;
    }
}


void request_validator::vector_to_map(request &req) {
    // when there is no request to deal with, just return.
    if(req.headers.size() == 0)
        return;
    // req.header is vector. and this process is converting the
    // data(saved into vector) to the data(save into unordered_map)
    for(int i =0; i < req.headers.size(); i++){
        std::string name_ = req.headers[i].name;
        std::string value_ = req.headers[i].value;
        req.headers_map[name_] = value_;
    }
}


bool request_validator::is_ascii(int c) 
{ return c >= 0 && c <= 127; }

bool request_validator::is_control(int c)
{ return (c >= 0 && c <= 31) || (c == 127); }

bool request_validator::is_num(int c)
{ return c >= '0' && c <= '9'; }

bool request_validator::is_special(int c) {
    switch (c) {
    case '(': case ')': case '<': case '>': case '@':
    case ',': case ';': case ':': case '\\': case '"':
    case '/': case '[': case ']': case '?': case '=':
    case '{': case '}': case ' ': case '\t':
        return true;
    default:
        return false;
    }
}
