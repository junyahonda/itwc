#ifndef SESSION_H
#define SESSION_H

#include <array>
#include <map>
#include <memory>
#include <boost/asio.hpp>
#include "data_structures.h"
#include "make_response.h"
#include "request_validation.h"
#include "handle_dispatch.h"

using boost::asio::ip::tcp;
class session_manager;
class base_request_handler;
// class handle_dispatch;

class session : public std::enable_shared_from_this<session> {
public:
    // restrict copy/assignment constructors
    session(const session&) = delete;
    session& operator=(const session&) = delete;


    // Constructor
    //   Arguments: socket (boost::asio::ip::tcp::socket) 
    //              manager (session_manager&)
    //              req_handler (base_request_handler&)
    //   Output: initialize data members
    explicit session(tcp::socket socket, 
                     session_manager &manager, 
                     std::map<std::string, base_request_handler*> &req_handler);


    void start();
 
    void stop();


    // Argument : -
    // Output   : -
    // this function is mainly doing the following process 
    // 1, read from socket, 
    // 2, convert bypte to string,
    // 3, parsing : find type and do process with the given type
    void handle_read();

    // Argument : -
    // Output   : -
    // the function is called at the end of the process. 
    // it transfers the response to socket
    void handle_write();


    // Argument : -
    // Output   : (string) type
    // the function is to find a type of the request
    // and it is counting the number of '/' in the request
    // the number of slash can be the index to check the validity of the request. 
    // if invalid, it returns empty string.
    // if valid, it returns the type of the request.
    std::string find_type();

    // Argument : (string)type
    // Output   : true/false
    // if type is indicating for echo, return true.
    bool is_echo(std::string &type);

    // Argument : (string) type passing by referenece
    // Output   : true if the given type exist in request_handler_
    // since the type of static can be stored as static1, static2, or static3
    // with iteration, it check whether the given type exists or not.
    bool is_static(std::string &type);

    // Argument : type
    // Output   : -
    // the function finds the given type into the map,
    // remove only the path in the request
    void remove_path(std::string &type);

    void print_message();

    // Argument : array, byte passing by reference
    // Output   : -
    // it is padding any missing \r before \n
    void padding(std::array<char, 2048> &arr, std::size_t& bytes_transferred);

    // Argument : byte 
    // Output   : -
    // the function takes bytes
    // it converts to string and returns it.
    std::string convert_to_str(std::size_t bytes_transferred);

    // Argument : string 
    // Output   : boolean
    // the function takes entire content of POST request
    // and save the contents to specified directories
    bool createSet(std::string &data);

private:
    // data members

    // socket for this session
    tcp::socket socket_;

    // maximum length to read in
    enum { max_length = 1024 };

    // buffer for data to read in
    std::array<char, max_length> data_;

    // manager class for this session
    session_manager &session_manager_;

    // request handler
    std::map<std::string, base_request_handler*> &request_handler_;

    // request from client
    request request_;

    // request validator
    request_validator request_validator_;

    // response object to be send to client
    response response_;

    // handle_dispatch object to generate response by calling corresponding request handlers
    handle_dispatch dispatch_;

};

typedef std::shared_ptr<session> session_ptr;

#endif // SESSION_H
