#ifndef MAKE_RESPONSE_H
#define MAKE_RESPONSE_H

#include <string>
#include <vector>
#include <boost/asio.hpp>
#include "data_structures.h"
#include "base_request_handler.h"
#include <map>


struct response {
    // data members

    // The status types. this will indicate which
    // status needs to be output
    enum status_type : size_t {
        ok = 200,
        created = 201,
        accepted = 202,
        no_content = 204,
        multiple_choices = 300,
        moved_permanently = 301,
        moved_temporarily = 302,
        not_modified = 304,
        bad_request = 400,
        unauthorized = 401,
        forbidden = 403,
        not_found = 404,
        internal_server_error = 500,
        not_implemented = 501,
        bad_gateway = 502,
        service_unavailable = 503
    } status; 

    // valid headers that are received and stored the headers
    // in to unordered map
    // It is also included in the response
    // The previous version was vector, but for following
    // common api, we stored the headers into unordered map.
    std::map <std::string, std::string> header_map;

    // data to be sent out (array of bytes) by included
    // in the response message
    std::string data;


    // methods

    // to_buffers
    //   Arguments: None  
    //   Output: bufferrs (vector<const_buffer>)
    // convert the current response objecct to a vector of
    // constant buffer. because the buffers cannot own
    // underlying mem blocks, the response object must be
    // valid throughout the write operations
    std::vector<boost::asio::const_buffer> to_buffers();


    // status_to_response
    //   Arguments: status (status_type)
    //   Output: return_resp (response)
    // convert the status_type to a corresponding response object
    static response status_to_response(status_type status);


    // status_to_response_echo
    //   Arguments: status (status_type)
    //   Output: return_resp (response)
    // convert the status_type to a corresponding response object
    // as echo message (copy of request message)
    static response status_to_response_echo(status_type status, const request &req);

    // response_status
    // Arguments: none
    // Output: response
    // generates a response when status is called
    static response response_status();

};


#endif // MAKE_RESPONSE_H
