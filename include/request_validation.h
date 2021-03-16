#ifndef REQUEST_VALIDATION_H
#define REQUEST_VALIDATION_H
#include <tuple> // should we replace this later?
#include <iostream>

struct request;

class request_validator {
public:
    // Constructor
    //   Arguments:  
    //   Output: 
    //
    request_validator();   


    // reset
    //   Arguments:  
    //   Output: 
    //  
    void reset();


    // type of results
    enum request_result { success, fail, in_progress };


    // parse <InputIterator>
    //   Arguments: req (request&)
    //              begin (InputIterator)
    //              end (InputIterator)
    //   Output: tuple of (request_result, InputIterator)
    //   iterates throrugh each input char and append it to the
    //   request object if they are valid at the current state/step
    //   then return the result with the iterator as tuple
    template <typename InputIterator>
    std::tuple<request_result, InputIterator> parse(request &req,
                                                 InputIterator begin, 
                                                 InputIterator end) {
        while (begin != end)
        {
            request_result result = validate(req, *begin++);
            if (result == success || result == fail) {
              // the function (vector_to_amp)
              // is iterating all the data(into vector) 
              // and saving it into unordered_map
              // Saving it into unordered_map for the format of common API
              // It is processed at the end of the validate(the end of the request).
              vector_to_map(req);
              return std::make_tuple(result, begin);
            }
        }
        return std::make_tuple(in_progress, begin);
    }

private:
    // validate
    //   Arguments: req (request&)
    //              input (char)
    //   Output: request_result
    //   checks if the input character is valid at the current
    //   state/step of the parsing
    request_result validate(request &req, char input);

    // saving data into unordered_map
    void vector_to_map(request &req);

    // is_ascii
    //   Arguments: c (int)
    //   Output: true if c is ascii character
    static bool is_ascii(int c);


    // is_control
    //   Arguments: c (int)
    //   Output: true if c is control character
    static bool is_control(int c);


    // is_special
    //   Arguments: c (int)
    //   Output: true if c is special character
    static bool is_special(int c);


    // is_num
    //   Arguments: c (int)
    //   Output: true if c is number character
    static bool is_num(int c);


    // indicates parser's each state/step
    enum steps {
        start_init,
        start,
        get_uri,
        get_http_version_h,
        get_http_version_t_1,
        get_http_version_t_2,
        get_http_version_p,
        get_http_version_slash,
        get_http_version_major_start,
        get_http_version_major,
        get_http_version_minor_start,
        get_http_version_minor,
        get_newline_before_header,
        get_header_start,
        get_header_space,
        get_header_name,
        get_space_after_header,
        get_header_value,
        get_newline_after_header,
        get_newline_end
    } current_step_;
};


#endif // REQUEST_VALIDATION_H
