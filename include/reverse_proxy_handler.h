#ifndef REVERSE_PROXY_HANDLER_H
#define REVERSE_PROXY_HANDLER_H

#include "base_request_handler.h"
#include "config_parser.h"
#include <memory>
#include "boost_networking_injector.h"

using tcp = boost::asio::ip::tcp;

// derived class for base_request_handler
class reverse_proxy_handler: public base_request_handler {
public:

    reverse_proxy_handler& operator=(const reverse_proxy_handler&) = delete;


    // Constructor
    //   Arguments: location prefix, and NginxConfig 
    //   Output: initialize root_path_
    //   it is the constructor to initialize the variables for starting the handler
    //   in the beginning. 
    explicit reverse_proxy_handler(std::string location_prefix, NginxConfig &config, std::string host, int port);

    // Argument : location prefix, NginxConfig object
    // Output   : the class object
    static reverse_proxy_handler* Init(std::string location_prefix, NginxConfig &config);

    //   Arguments: req (request&)
    //   Output: it returns the proper response 
    //           based on request
    // it scans the request format and checks whether it is valid format or not.
    // depending on the request, it generates the proper response
    virtual response do_handle(request &req);

    response do_handle(request &req, std::shared_ptr<boost_networking_injector> injector);


    //checks that the format of the config is correct for a reverse_proxy_handler
    //Then configures host_ip and port using the config
    static bool config_check(NginxConfig *config, std::string &host, int &port);


private:

    //IP of the first host we will connect to
    std::string host_;

    //port we want to connect to
    int port_;

    //location_prefix_
    std::string location_prefix_;
};


#endif // REVERSE_PROXY_HANDLER_H
