#ifndef BASE_REQUEST_HANDLER_H
#define BASE_REQUEST_HANDLER_H

#include <string>
#include <vector>
#include "data_structures.h"
#include "config_parser.h"
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/chrono.hpp>



struct request;
struct response;

// base class for request_handler
class base_request_handler {
    public:
        base_request_handler& operator=(const base_request_handler&) = delete;


        // Constructor
        //   Arguments: location prefix, and NginxConfig 
        //   Output: initialize root_path_
        //   it is the constructor to initialize the variables for starting the handler
        //   in the beginning. 
        explicit base_request_handler(std::string location_prefix, NginxConfig &config);

        // do_handle
        // Argument : request object(passing by reference)
        // Output   : response

        // do_handle is the main function in the class
        // However, in the base, it does not have any functionality but it is
        // pure virtual function. It is only to provide the base type and format
        // of the do_handles from subclasses
        virtual response do_handle(request &req) = 0;


        /******************************************************
        From here, all the functions are related to /status
        *******************************************************/

        // total_handler
        // Argument :
        // Output : total number of handlers to be called
        // the function is returning the total number of called handlers.
        static int total_handler();

        // inc_number_of_handlers
        // Argument : (string) a type of handler(eg, static, echo, not_found, status)
        // Output  : true or false.
        // when the function is called, it is increasing 1 of the number in
        // the called handler. If an invalid parameter has been passed,
        // it returns false, or else, returns true.
        static bool inc_number_of_handlers(std::string type_of_handler);

        // get_number_of_type
        // Argument : (string) a type of handler (eg, static, echo, not_found, status)
        // Output : an integer (the number of handler to be called)
        // It returns the number of a called handler
        static int get_number_of_type(std::string type_of_handler);
        
        // get_response_code_map
        // Argument : -
        // Output : reponse code map,
        // it is just simply returning the map from private. 
        static std::map<std::string, int> get_response_code_map();

        // add_to_code_map
        // Arguement : (string) response code (eg, 200, 400, 404)
        // Output    : -
        // it is creating the map, it will be helping to figure out
        // how many error codes have been shown when the server is starting.
        static void add_to_code_map(std::string response_code);

        // get_number_of_url
        // Argument : reponse code (ex 200 400 404) 
        // Output : -
        // it is just simply returning the map(url , number to be called)
        static std::map<std::string, int> get_number_of_url();

        // add_to_url_map
        // Argument : - 
        // Output   : map of url (<string> url, <int>number to be called)
        // once a url is called, the number is increasing and save data into map
        static void add_to_url_map(std::string url);

        // get_all_handlers
        // Argument : -
        // Output   : map that contains headers and its path 
        // get all available handlers from the map  (handler name, path name)
        static std::map<std::string,std::vector<std::string>> get_all_handlers();
        
        // add_to_all_handlers
        // Argument : (string) handler, (string) its path  
        // Output : - 
        // it is adding the handler name and the path into map
        static void add_to_all_handlers(std::string handler_name, std::string path_name);
        //static std::vector<std::string> get_all_handlers_result(std::string handler_name);

        // get_root_path
        // Argument : -
        // Output : string 
        // returns root_path of the handler
        std::string get_root_path();

    protected:
        // root path that is assigned
        std::string location_prefix_;
        NginxConfig &config_;
        std::string root_path_;

        // convert all hex character in url to regular ascii chars


        // find_root
        // Argument : NginxConfig
        // Output   : a mapped path
        // The function is kept iterating the Nginxconfig;
        // find a token to save "root" and returning the following
        // token, which is the mapped path.
        // if the root is not found, it returns \ which is 
        // for representing of 404 
        std::string find_root(NginxConfig *conf);


        // clean_url
        //   Arguments: original (string&)
        //              result (string&)
        //   Output: boolean whether cleaning url was successful
        //   converts all hex chars in url to regaulr ascii
        static bool clean_url(const std::string &original,
                                    std::string &result);

        // read_url
        std::string real_url(std::string url);

        

    private:
        // for STATUS requests.

        static int num_echo_;
        static int num_static_;
        static int num_status_;
        static int num_not_found_;
        static int num_proxy_;
        static int num_health_;
        static int num_itwc_;
        static std::map<std::string, int> response_code_map_;  
        static std::map<std::string, int> url_map_;
        static std::map<std::string,std::vector<std::string>> all_handlers_;
        static boost::mutex mutex_;
};


#endif // BASE_REQUEST_HANDLER_H