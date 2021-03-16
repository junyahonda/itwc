#ifndef ITWC_HANDLER_H
#define ITWC_HANDLER_H

#include "base_request_handler.h"
#include <map>

// derived class for base_request_handler
class itwc_handler: public base_request_handler {
public:

    itwc_handler& operator=(const itwc_handler&) = delete;


    // Constructor
    //   Arguments: location prefix, and NginxConfig 
    //   Output: initialize root_path_
    //   it is the constructor to initialize the variables for starting the handler
    //   in the beginning. 
    explicit itwc_handler(std::string location_prefix, NginxConfig &config);

    // Argument : location prefix, NginxConfig object
    // Output   : the class object as pointer
    static itwc_handler* Init(std::string location_prefix, NginxConfig &config);

    //   Arguments: req (request&)
    //   Output: it returns the proper response 
    //           based on request
    virtual response do_handle(request &req);

    //   Arguments: set_name (string&)
    //   Output: returns true if the set exists
    bool is_dir_exist(std::string &set_name);

    //   Arguments: set_name (string&)
    //   Output: returns the dir of the set
    std::string get_dir(std::string &set_name);

    //   Arguments: set_name (string&), dir_name (string&)
    //   Output: returns true if successfully saved new set dir
    bool set_dir(std::string &set_name, std::string &dir_name);

private:
    std::map<std::string,std::string> set_dirs_;

    //   Arguments: winner (string&)
    //   Output: N/A
    void add_winner(std::string &winner);
};


#endif // ITWC_HANDLER_H