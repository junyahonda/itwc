#include "reverse_proxy_handler.h"
#include "config_parser.h"
#include "mime_types.h"
#include "make_response.h"
#include "log_handler.h"
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>
#include <boost/asio/write.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ip/tcp.hpp>
#include "response_parser.h"
#include "html_parser.h"
#include "boost_networking_injector.h"
#include <memory>

#define GetCurrentDir getcwd

using tcp = boost::asio::ip::tcp;


// Constructor
reverse_proxy_handler::reverse_proxy_handler(std::string location_prefix, NginxConfig &config, std::string host, int port)
                     :base_request_handler(location_prefix, config), location_prefix_(location_prefix), host_(host), port_(port) {

}

reverse_proxy_handler* reverse_proxy_handler::Init(std::string location_prefix, NginxConfig &config) {
    handle_log(4, "Initialized proxy handler");
    std::string host;
    int port;
    if(!config_check(&config, host, port))
        return nullptr;

    return new reverse_proxy_handler(location_prefix, config, host, port);
}

bool reverse_proxy_handler::config_check(NginxConfig *config, std::string &host, int &port) {

    //check only 2 statements in configuration (host and port)
    if(config->statements_.size() != 2){
        handle_log(4, std::string("reverse_proxy_handler configuration incorrect. Bad number of configuration statements: ") + std::to_string(config->statements_.size()));
        return false;
    }

    bool host_exists = false;
    bool port_exists = false;

    //check each statement is correctly configured
    for (const auto& statement : config->statements_) {
    
        if(statement->tokens_.size() != 2){
            handle_log(4, std::string("reverse_proxy_handler configuration incorrect. Bad number of tokens in a statement: ") + std::to_string(statement->tokens_.size()));
            return false;
        }

        std::string config_type = (statement->tokens_)[0];

        if(config_type == "host"){
            std::string host_string = (statement->tokens_)[1];

            //host should be surrounded by parentheses and contain at least one character
            if(host_string.size() <= 2 || host_string[0] != '\"' || host_string[host_string.size() - 1] != '\"'){
                handle_log(4, std::string("Reverse_proxy_handler host statement poorly configured: ") + host_string + ". Should be quoted and have at least one character");
                return false;
            }
            host = host_string.substr(1, host_string.size() - 2);
            // Remove ending slash if one is provided
            if (host[host.size() - 1] == '/') {
                host.substr(0, host.size() - 2);
            }
            host_exists = true;
        }
        else if(config_type == "host_port"){

            //check if port in configuration file is an integer
            //code from git-gud-with-gerrit code base src/nginx_config_parser.cc GetPortInternal() function
            std::string port_string = (statement->tokens_)[1];

            if (port_string == "" || port_string.find_first_not_of("0123456789") != std::string::npos || std::stoi(port_string) == 0){
                handle_log(4, std::string("Reverse_proxy_handler port statment poorly configured: ") + port_string + ". Should be a postive integer");
                return false;
            }
            port = std::stoi(port_string);
            port_exists = true;
        }
        else{
            handle_log(4, std::string("Bad config statment for reverse_proxy_handler. No configuration for ") + config_type + ". Only configurations for host and port");
            return false;
        }
    }

    //check that host and port are configured
    if(!host_exists)
        handle_log(4, std::string("reverse_proxy_handler configuration error: No host detected"));

    if(!port_exists)
        handle_log(4, std::string("reverse proxy handler configuration error: No port detected"));

    return host_exists && port_exists;
}

//Used for dependency injection testing.
response reverse_proxy_handler::do_handle(request &req) {
    std::shared_ptr<boost_networking_injector> injector =
        std::shared_ptr<boost_networking_injector>(new boost_networking_injector());

    try {
    response out = do_handle(req, injector);
    injector.reset();
    return out;
    }
    catch (boost::system::system_error e) {
        handle_log(5, "Failed to handle proxy request");
        response rsp;
        rsp.status = response::status_type::internal_server_error;
        injector.reset();
        return rsp;
    }
}

response reverse_proxy_handler::do_handle(request &req, std::shared_ptr<boost_networking_injector> injector) {
    response resp;
    boost::system::error_code error; //Error handling

    std::string request_path;
    log_data log;
    log.request_handler = "proxy";
    log.ip_address = req.ip;
    if (!clean_url(req.uri, request_path)) {
        // unable to clean the url which indicates the
        // invalid url 
        resp = response::status_to_response(response::bad_request);
        base_request_handler::inc_number_of_handlers("proxy");
        base_request_handler::add_to_code_map(std::to_string((size_t)resp.status));
        base_request_handler::add_to_url_map(request_path);
        log.response_code = std::to_string(response::bad_request);
        handle_machine_log(log, 4, "[Message] invalid url - cannot clean url");
        return resp;
    }

    // another invalid path (url) cases
    // 1. path cannot be empty
    // 2. path cannot start other than '/'
    // 3. path cannot contain ".."
    if (request_path.empty()   || 
        request_path[0] != '/' || 
        request_path.find("..") != std::string::npos) {
        // bad url detected
        resp = response::status_to_response(response::bad_request);
        base_request_handler::inc_number_of_handlers("proxy");
        base_request_handler::add_to_code_map(std::to_string((size_t)resp.status));
        base_request_handler::add_to_url_map(request_path);
        log.response_code = std::to_string(response::bad_request);
        handle_machine_log(log, 4, "[Message] invalid url - illegal path");
        return resp;
    }

    std::string current_host = host_;
    std::string current_port = std::to_string(port_);

    // Refuse gzip
    req.headers_map["Accept-Encoding"] = " ";
    // Force closing the connection
    req.headers_map["Connection"] = "close";

    std::string request_string = 
        req.method + " " + req.uri.substr(location_prefix_.size()) + " HTTP/1.0\r\n";
    for (const auto header_name_value_pair : req.headers_map) {
        auto header_name = header_name_value_pair.first;
        auto header_value = header_name_value_pair.second;
        request_string += header_name + ": " + header_value + "\r\n";
    }
    request_string += "\r\n";

    handle_log(2, "Sending to host " + current_host + ": " + request_string);

    // Due to no body_ field (Common API) we must parse for the body manually.
    
    // Try finding Content-Length to identify the rest of the request
    size_t content_length = std::string::npos;
    size_t start_of_body = req.raw.find("\r\n\r\n");
    start_of_body = (start_of_body == std::string::npos) ? std::string::npos : start_of_body + 4;

    if (req.headers_map.find("Content-Length") != req.headers_map.end()) {
        content_length = std::atoi(req.headers_map.at("Content-Length").c_str());
        if (content_length < 0) {
            // invalid content length, return bad request
            resp = response::status_to_response(response::bad_request);
            base_request_handler::inc_number_of_handlers("proxy");
            base_request_handler::add_to_code_map(std::to_string((size_t)resp.status));
            base_request_handler::add_to_url_map(request_path);


            log.response_code = std::to_string(response::bad_request);
            handle_machine_log(log, 4, "[Message] Content length is not acceptable");

            return resp;
        }
    }
    handle_log(1, std::to_string(start_of_body)); 
    request_string += (req.raw.substr(start_of_body, content_length));

    while (true) {
        injector->connect(current_host, current_port);
        injector->communicate(request_string, resp, current_host, current_port);
    
        if(resp.status == response::moved_permanently || resp.status == response::moved_temporarily) {
            // Check if key exists in map code from URL:
            // https://stackoverflow.com/questions/1939953/how-to-find-if-a-given-key-exists-in-a-c-stdmap
            if (resp.header_map.find("Location") == resp.header_map.end()){
                resp = response::status_to_response(response::bad_request);
                base_request_handler::add_to_url_map(request_path);
                handle_log(5, std::string("Bad redirect (301 or 302) response from host: ") + current_host + ":" + current_port + ". No location specified");
                exit(1);
            }
            else{
                size_t separator_position = resp.header_map["Location"].find_last_of(":");

                // Check if http:// is caught, not the port
                std::string new_host;
                if (resp.header_map["Location"].find("http://") == 0 && separator_position == 4) {
                    separator_position = std::string::npos;
                    new_host = resp.header_map["Location"];
                }
                else {
                    new_host = "http://" + resp.header_map["Location"].substr(0, separator_position);
                }

                std::string new_port = (separator_position == std::string::npos) ? "80" : resp.header_map["Location"].substr(separator_position + 1);

                handle_log(2, "Redirecting with location " + resp.header_map["Location"] + ". current host = \"" + current_host + ":" + current_port + "\". new host = \"" + new_host + ":" + new_port + "\"");

                // If we are supposed to redirect to a different host, break if not the server, else request location_prefix + path
                
                if ((new_host.find(current_host) != 0 && current_host.find("http://") == 0)
                    || (new_host.find("http://" + current_host) != 0) && current_host.find("http://") != 0) {
                    std::string request_host = req.headers_map["Host"];
                    std::string request_port = "";

                    if (request_host.find("http://") != 0) {
                        // host is not prepended with http://, add it in
                        request_host = "http://" + request_host;
                    }

                    size_t host_separator_pos = request_host.find_last_of(":");
                    if (host_separator_pos != 4 && host_separator_pos != std::string::npos) {
                        // There is a port number
                        request_port = ":" + request_host.substr(host_separator_pos + 1);
                        request_host = request_host.substr(0, host_separator_pos);
                    }

                    handle_log(2, "Request Host: " + request_host);
                    if (new_host.find(request_host) == 0 && current_port == new_port) {
                        handle_log(2, request_host + " was referenced as the redirect");
                        resp.header_map["Location"] = request_host + request_port + location_prefix_ + new_host.substr(request_host.size());
                        handle_log(2, "Location is now " + resp.header_map["Location"]);
                    }

                    break;
                }

                current_host = new_host;
                current_port = new_port;

            }
        }
        else {
            break;
        }
    }

    // Final response here
    handle_log(4, "Server response was " + std::to_string(resp.status) + " | location: " + resp.header_map["Location"]);
    if (resp.status == response::status_type::ok &&
        resp.header_map.find("Content-Type") != resp.header_map.end() &&
        resp.header_map.at("Content-Type").find("text/html") != std::string::npos) {
            
        resp.data = html_parsing::update_tag_attribute(resp.data, location_prefix_, "href");
        resp.data = html_parsing::update_tag_attribute(resp.data, location_prefix_, "src");
        resp.header_map["Content-Length"] = std::to_string(resp.data.size());
    }

    // TODO: Make sure this is the right url and port for the deployed server (80 if not specified)
    resp.header_map["Host"] = req.headers_map["Host"];

    base_request_handler::inc_number_of_handlers("proxy");
    base_request_handler::add_to_code_map(std::to_string((size_t)resp.status));
    base_request_handler::add_to_url_map(request_path);


    log.response_code = std::to_string(response::ok);
    handle_machine_log(log, 2, "[Message] Response created successfully");

    return resp;
}
