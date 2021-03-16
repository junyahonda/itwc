#ifndef NOT_FOUND_HANDLER_H
#define NOT_FOUND_HANDLER_H

#include "base_request_handler.h"


// the class is the sub_class from base_request_handler
class not_found_handler: public base_request_handler {
public:

    not_found_handler& operator=(const not_found_handler&) = delete;
    
    // Constructor
    //   Arguments: location prefix, and NginxConfig 
    //   Output: initialize root_path_
    //   it is the constructor to initialize the variables for starting the handler
    //   in the beginning. 
    explicit not_found_handler(std::string location_prefix, NginxConfig &config);

    // Argument : location prefix, NginxConfig object
    // Output   : the class object
	static not_found_handler* Init(std::string location_prefix, NginxConfig &config);
	
    //   Arguments: req (request&)
    //   Output: it returns the proper response 
    //           based on request
    // it scans the request format and checks whether it is valid format or not.
    // depending on the request, it generates the proper response
	virtual response do_handle(request &req);

};

#endif // NOT_FOUND_HANDLER_H