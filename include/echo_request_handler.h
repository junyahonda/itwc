#ifndef ECHO_REQUEST_HANDLER_H
#define ECHO_REQUEST_HANDLER_H

#include "base_request_handler.h"

// derived class for base_request_handler
class echo_request_handler: public base_request_handler {
public:
    echo_request_handler& operator=(const echo_request_handler&) = delete;


    // Constructor
    //   Arguments: location prefix, and NginxConfig 
    //   Output: initialize root_path_
    //   it is the constructor to initialize the variables for starting the handler
    //   in the beginning. 
    explicit echo_request_handler(std::string location_prefix, NginxConfig &config);

    // Argument : location prefix, NginxConfig object
    // Output   : the class object
    static echo_request_handler* Init(std::string location_prefix, NginxConfig &config);




    //   Arguments: req (request&)
    //   Output: it returns the proper response 
    //           based on request
    // it scans the request format and checks whether it is valid format or not.
    // depending on the request, it generates the proper response
    virtual response do_handle(request &req);

};


#endif // ECHO_REQUEST_HANDLER_H