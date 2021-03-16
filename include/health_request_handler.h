#ifndef HEALTH_REQUEST_HANDLER_H
#define HEALTH_REQUEST_HANDLER_H

#include "base_request_handler.h"

// derived class for base_request_handler
class health_request_handler: public base_request_handler {
public:
    health_request_handler& operator=(const health_request_handler&) = delete;


    // Constructor
    //   Arguments: location prefix, and NginxConfig 
    //   Output: creates health request handler with given prefix location 
    explicit health_request_handler(std::string location_prefix, NginxConfig &config);

    // Argument : location prefix, NginxConfig object
    // Output   : the class object
    static health_request_handler* Init(std::string location_prefix, NginxConfig &config);


    //   Arguments: req (request&)
    //   Output: it returns the proper response as health request handler
    // it returns 200 OK message.
    virtual response do_handle(request &req);

};


#endif // HEALTH_REQUEST_HANDLER_H