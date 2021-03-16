#ifndef SERVER_H
#define SERVER_H

#include <boost/asio.hpp>
#include <vector>
#include <map>
#include <string>
#include "session.h"
#include "session_manager.h"
#include "base_request_handler.h"
struct handler_config;

using boost::asio::ip::tcp;

class server
{
public:

    // disallow copy constructor or assigment
    server(const server&) = delete;
    server& operator=(const server&) = delete;


    // Constructor
    //   Arguments: addr (string&)
    //              port (string&)
    //              root_path (string&)
    //   Output: Initialize the server
    explicit server(const std::string &addr,
                    const std::string &port,
                    /*std::map<std::string, std::string> &paths*/
                    std::vector<handler_config> list_handlers);

    // Destructor
    ~server();

    // run
    //   Arguments: None
    //   Output   : None
    // running the io_service loop
    void run();

private:

    //   Arguments:  
    //   Output: 
    void start_accept();


    //   Arguments:  
    //   Output: 
    //   
    void wait_stop();
    
    // Arguments : (int)type, 
    //             (string) prefix,
    //             NginxConfig object
    // Output    : base_request_handler*
    // depends on the type value, the function returns the corresponding request handler's init 
    base_request_handler* createHandler(int type, std::string location_prefix, NginxConfig &config);

    // Arguments : vector of handler_config
    // Output    : -
    // it is the function to set up initial handlers.
    // the vector is iterated and calls createHandler to set up the system
    //  in the beginning.
    void init_handlers(std::vector<handler_config> handler_list);

    
    // data members

    // io_service for asynchronous operations
    boost::asio::io_service io_service_;

    // signals to register terminations
    boost::asio::signal_set signals_;

    // acceptor to be used for listening connections
    tcp::acceptor acceptor_;

    // manager to maintain sessions
    session_manager session_manager_;

    // next socket to accept
    tcp::socket socket_;

    // handler for requests
    std::map<std::string, base_request_handler*> request_handler_;

    // size of the thread pool
    std::size_t thread_pool_size_;
};


#endif // SERVER_SERVER_H