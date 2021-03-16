#pragma once

#include <map>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio.hpp>
#include "make_response.h"

// Used to communicate without having to create a socket, io_service, resolver, etc.
class boost_networking_injector {
    public:
    
    boost_networking_injector();
    ~boost_networking_injector();
    virtual void connect(std::string host, std::string port) const;
    virtual void communicate(std::string request_string, response& resp, std::string host, std::string port) const;
    private:
    boost::asio::ip::tcp::socket* socket_;
    boost::asio::ip::tcp::resolver* resolver_;
    boost::asio::io_service* io_service_;
};
