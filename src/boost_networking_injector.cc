#include "boost_networking_injector.h"
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio.hpp>
#include "make_response.h"
#include "response_parser.h"
#include "log_handler.h"

boost_networking_injector::boost_networking_injector() {
    io_service_ = new boost::asio::io_service();
    socket_ = new boost::asio::ip::tcp::socket(*io_service_);
    resolver_ = new boost::asio::ip::tcp::resolver(*io_service_);
    io_service_->run();
}
boost_networking_injector::~boost_networking_injector() {
    io_service_->stop();
    delete resolver_;
    delete socket_;
    delete io_service_;
}
void boost_networking_injector::connect(std::string host, std::string port) const {
    boost::system::error_code error; //Error handling
    boost::asio::ip::tcp::resolver::query host_dns_name(host, port);
    
    //get the host's ip_address from its name. Code from URL: https://www.tutorialspoint.com/how-to-get-the-ip-address-of-local-computer-using-c-cplusplus
    try {
        boost::asio::connect(*socket_, resolver_->resolve(host_dns_name));
    }
    catch (boost::system::system_error e) {
        handle_log(5, std::string("Error: Host ") + host + ":" + port + " could not be connected to: "); 
        exit(1);
    }
}

void boost_networking_injector::communicate(std::string request_string, response& resp, std::string host, std::string port) const {
    response_parser rsp_parser;
    boost::system::error_code error; //Error handling
    boost::asio::write(*socket_, boost::asio::buffer(request_string.c_str(), request_string.size()), error);
    if(error) {
        handle_log(5, std::string("Error occurred while writing to host: ") + host + ":" + port + ". Error" + error.message());
        exit(1);
    }


    const int kMaxLength = 1024;

    char read_buf[kMaxLength];
    std::string server_response;
    std::string excess; // Not necessary since we are forcing no TE=chunked by using HTTP/1.0

    handle_log(2, "Starting to read server response");

    do {
        // Data reading code from session.cc git-gud-with-gerrit change: URL: https://code.cs130.org/c/git-gud-with-gerrit/+/3005
        size_t bytes_read = 0;
        try{
            bytes_read = socket_->read_some(boost::asio::buffer(read_buf, kMaxLength));
            server_response.append(read_buf, bytes_read);
        }
        catch(boost::system::system_error e){
            if (std::string(e.what()) != std::string("read_some: End of file")) {
                handle_log(5, std::string("Exception occurred while reading from host: ") + host + ":" + port + ". Exception: \"" + e.what() + "\"");
                exit(1);
            }
            else
                break;
        }
    } while(true);
    handle_log(2, "Reading done");
    socket_->shutdown(socket_->shutdown_both);
    socket_->close();

    if (!rsp_parser.parse(server_response, resp, excess)) {
        resp = response::status_to_response(response::bad_request);
    }    
}
