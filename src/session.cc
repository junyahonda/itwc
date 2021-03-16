#include "session.h"
#include <utility>
#include <bits/stdc++.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <vector>
#include <fstream>
#include <boost/asio.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include "session_manager.h"
#include "log_handler.h"
#include "echo_request_handler.h"
#include "handle_dispatch.h"
#include "static_request_handler.h"
#include "itwc_handler.h"


session::session(boost::asio::ip::tcp::socket socket, 
                 session_manager &manager, 
                 std::map<std::string, base_request_handler*> &req_handler)
        :socket_(std::move(socket)),
         session_manager_(manager),
         request_handler_(req_handler),
         request_(),
         request_validator_(),
         response_(),
         dispatch_(req_handler) {}



void session::start(){ 
    handle_log( 2, "Connection established with a client.");
    handle_read(); 
}



void session::stop(){ 
    // handle_log(socket_.remote_endpoint().address().to_string(), 2, "Closing connection with a client...");
    socket_.close(); 
    handle_log(2, "Successfully closed connection with a client.");
}


void session::print_message() {
    std::string message = "Request info: ";
    message += "method: " + request_.method + ", ";
    message += "uri: " + request_.uri;
    handle_log(2, message);
}



void session::padding(std::array<char, 2048> &arr, std::size_t& bytes_transferred) {
    std::size_t origin_size = bytes_transferred;
    
    int j = 0;
    for (int i = 0; i < origin_size; ++i) {
        if (data_[i] == '\n' && (i == 0 || data_[i-1] != '\r')) {
            arr[j++] = '\r';
            ++bytes_transferred;
        }
        arr[j++] = data_[i];
    }
}


std::string session::convert_to_str(std::size_t bytes_transferred) {
    std::string str = "";
    for (int i = 0; i < bytes_transferred; ++i) {
        str += data_[i];
    }
    return str;
}


std::string session::find_type() {
    int first_slash = 0;
    while (first_slash < request_.uri.size()) {
        if (request_.uri[first_slash] == '/') 
            break;
        else 
            ++first_slash;
    }
    
    if (first_slash >= request_.uri.size()) 
        return "";
    
    int second_slash = first_slash + 1;
    while (second_slash < request_.uri.size()) {
        if (request_.uri[second_slash] == '/')
            break;
        else
            ++second_slash;
    }

    if (second_slash >= request_.uri.size()) 
        return "";

    return request_.uri.substr(first_slash, second_slash - first_slash);
}

bool session::is_echo(std::string &type) 
{ return type == "/echo"; }


bool session::is_static(std::string &type) {
    if (request_handler_.find(type) == request_handler_.end())
        return false;
    return true;
}

void session::remove_path(std::string &type) {
    std::size_t pos = request_.uri.find(type);
    request_.uri = request_.uri.substr(pos + type.length());
}


void session::handle_read() {    
    // create a self pointer for current object
    auto self(shared_from_this());

    // read up to max_length bytes from socket_
    socket_.async_read_some(boost::asio::buffer(data_, max_length),
        [this, self](boost::system::error_code ec, std::size_t bytes_transferred) {
            // created local function to visualize things easier

            // if no error, parse the request
            if (!ec) {

                // With received data, call padding to pad any missing
                // \r chars before the \n
                request_.raw += convert_to_str(bytes_transferred);
                std::array<char, 2048> new_arr;
                padding(new_arr, bytes_transferred);
                request_validator::request_result result;
                std::tie(result, std::ignore) = request_validator_.parse(
                    request_, new_arr.data(), new_arr.data() + bytes_transferred);
                
                handle_log(1, "START " + request_.uri);
                // process the request if it is valid
                std::cout << "BYTES TRANSFERRED: " << std::to_string(bytes_transferred) << std::endl;

                if (result == request_validator::success) {
                    // server console should be able to observe the request and its headers
                    std::cout << "###################\n";
                    std::cout << "##CURRENT REQUEST##\n";
                    std::cout << request_.method << " " << request_.uri << " HTTP/" << request_.http_version_major << "." << request_.http_version_minor << std::endl;
                    for (auto it = request_.headers_map.begin(); it != request_.headers_map.end(); ++it) {
                        std::cout << it->first << ": " << it->second << std::endl;
                    }
                    std::cout << std::endl;
                    std::cout << "###################\n";

                    std::string request_body = "";
                    request_body.append(request_.raw.substr(request_.raw.find("\r\n\r\n")));

                    request_.ip = socket_.remote_endpoint().address().to_string();
                    response_ = dispatch_.match_handler(request_); /* calling match_handler finds longest url match and generate response */
                    if (request_.method == "POST") {
                        handle_log(2, "Retrieving images content from client");
                        const int kMaxLength = 1024;
                        size_t bytes_read = request_body.size();
                        char read_buf[kMaxLength];

                        const size_t size = std::stoi(request_.headers_map["Content-Length"]);
                        size_t packet_size = 0;
                        while (bytes_read < size) {
                            packet_size = socket_.receive(boost::asio::buffer(read_buf, kMaxLength));
                            bytes_read += packet_size;
                            // packet_size = socket_.read_some(boost::asio::buffer(read_buf, size));
                            request_body.append(read_buf, packet_size);                            
                        }

                        createSet(request_body);
                    }


                    handle_log(2, "This is a valid request.");
                    print_message();
                    handle_write();
                }

                // if request validation fails, return base request msg
                else if (result == request_validator::fail) {
                    std::string first_path = find_type();

    
                    if (is_echo(first_path)) {
                        response_ = response::status_to_response_echo(response::bad_request, request_);
                    } else {
                        response_ = response::status_to_response(response::bad_request);
                    }
                    handle_log(4, "This is not a valid request.");
                    print_message();
                    handle_write();
                }

                // otherwise, read from socket_
                else {
                    handle_read();
                }
            }

            // unless server is specifically aborted, stop this session
            // through current session manager
            else if (ec != boost::asio::error::operation_aborted) {
                session_manager_.stop(shared_from_this());
            }
        });

}


void session::handle_write() {
    // TODO: Fix ordering of socket teardown inside the async write handler below.
    // Specifically shutdown is being called before session_manager_.stop() which
    // calls session::stop which attempts to get information from the remote endpoint.
    // However due to the shutdown this information get fails with an exception that
    // kills the server. If session_manager_.stop is to be called, shutdown should not
    // be called before the logging message in session::stop

    // create a self pointer for current object
    auto self(shared_from_this());

    // transfer response to socket_
    boost::asio::async_write(socket_, response_.to_buffers(),
        [this, self](boost::system::error_code ec, std::size_t) {
            // local function to handle simpler
            
            // if no error found, shut down the socket
            if (!ec) {
                // Initiate graceful connection closure.
                boost::system::error_code ignored_ec;
                socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both,
                                 ignored_ec);
            }
            
            // unless aborted, stop this session using its manager
            if (ec != boost::asio::error::operation_aborted) {
                session_manager_.stop(shared_from_this());
            }
        });
}

bool session::createSet(std::string &data) {
    handle_log(2, "Creating the set and storing images");

    // important variables
    std::string set_name;
    std::map<std::string,std::string> files;


    // parse the set name
    size_t set_name_pos = data.find("set_name");
    std::string set_name_data = data.substr(set_name_pos);
    std::istringstream iss(set_name_data);
    size_t counter = 1;
    for (std::string line; std::getline(iss, line); ++counter) {
        if (counter == 3) {
            if (line[line.size()-1] == '\n')
                line = line.substr(0, line.size()-1);
            if (line[line.size()-1] == '\r')
                line = line.substr(0, line.size()-1);
            set_name = line;
            break;
        }
    }


    // save file contents into a map
    std::string curr_data = data;
    size_t pos1;
    size_t pos2;
    size_t newline;
    std::string file_name;
    std::string file_data;
    while (curr_data != "") {
        // get file name
        pos1 = curr_data.find("filename=");
        if (pos1 == std::string::npos)
            break;
        pos1 += 10;
        pos2 = pos1;
        while (curr_data[pos2] != '\"')
            ++pos2;
        file_name = curr_data.substr(pos1, pos2-pos1);

        // remove previous strings
        curr_data = curr_data.substr(pos2 + 1);

        // strip non-content data
        newline = curr_data.find("\r\n\r\n");
        curr_data = curr_data.substr(newline+4);

        // find the end of content and save
        pos1 = curr_data.find("------WebKitForm");
        file_data = curr_data.substr(0, pos1);

        // store it into a map
        files[file_name] = file_data;

    }
   
    // find the file path of itwc
    if (request_handler_.find("/itwc") == request_handler_.end())
        return false;
    base_request_handler *itwc_base = request_handler_["/itwc"];
    itwc_handler *itwc = static_cast<itwc_handler*>(itwc_base);

    std::string itwc_path = itwc->get_root_path();
    std::string set_path = itwc_path;
    set_path += "/sets/";
    set_path += set_name;

    if (itwc->set_dir(set_name, set_path) == false) {
        return false;
    }
    std::cout << "new directory: " << set_path << std::endl;
    if (mkdir(set_path.c_str(), 0777) == -1) 
        std::cerr << "Error :  " << std::strerror(errno) << std::endl; 
    else
        std::cout << "Directory created";

    set_path += "/";
    std::cout << "NEW SET IS " << set_name << std::endl;

    // append the new topic to the list of topics
    std::ofstream out_topic;
    out_topic.open("../data/ITWC/topics.txt", std::ios_base::app);
    out_topic << "\n"+set_name;


    size_t i = 0;
    std::string text_input = "";
    for (auto it = files.begin(); it != files.end(); ++it) {
        std::cout << "FILE #" << std::to_string(i) << ": " << it->first << std::endl;
        std::string full_path = set_path + it->first;

        std::ofstream wf(full_path, std::ios::out | std::ios::binary);
        if(!wf) {
            std::cout << "Cannot open file!" << std::endl;
            return false;
        }
        wf.write((it->second).c_str(), (it->second).size());
        wf.close();

        text_input.append(set_name);
        text_input.append("/");
        text_input.append(it->first);
        text_input.append("#");
        ++i;
    }
    text_input.pop_back();

    // create image info textfile
    std::string text_path = itwc_path;
    text_path += "/image_list/";
    text_path += set_name;
    text_path += ".txt";
    std::ofstream wf(text_path, std::ios::out | std::ios::binary);
    if(!wf) {
        std::cout << "Cannot open file!" << std::endl;
        return false;
    }
    wf.write(text_input.c_str(), text_input.size());
    wf.close();


    handle_log(2, "Set creation completed");
    return true;
}
