#include "base_request_handler.h"
#include "make_response.h"
#include "log_handler.h"
#include <iostream>

int base_request_handler::num_echo_      = 0;
int base_request_handler::num_static_    = 0;
int base_request_handler::num_status_    = 0;
int base_request_handler::num_not_found_ = 0;
int base_request_handler::num_proxy_     = 0;
int base_request_handler::num_health_    = 0;
int base_request_handler::num_itwc_      = 0;
std::map<std::string, int> base_request_handler::response_code_map_;  
std::map<std::string, int> base_request_handler::url_map_;
std::map<std::string,std::vector<std::string>> base_request_handler::all_handlers_;
boost::mutex base_request_handler::mutex_;

// constructor
base_request_handler::base_request_handler(std::string location_prefix, NginxConfig &config)
                     :location_prefix_(location_prefix),
                     config_(config)


{ 
    std::string path = find_root(&config);
    root_path_ = path.substr(1, path.size()-2);
    handle_log(1, "Root path: " + root_path_);
} 


std::string base_request_handler::find_root(NginxConfig *conf) {
    for (const auto& statement : conf->statements_) {
        if((statement->tokens_)[0] == "root") {
            return (statement->tokens_)[1];
        }
    }
    return "\"\"";
}

// param: request.uri
std::string base_request_handler::real_url(std::string url) {
    return url.substr(location_prefix_.size());
}


bool base_request_handler::clean_url(const std::string &original,
                                           std::string &result) {
    // reset the result string
    result.clear();
    result.reserve(original.size());

    // iterrate each character and clean them
    for (std::size_t i = 0; i < original.size(); ++i) {
        // hex values are expected in next 2
        if (original[i] == '%') {
            int converted = 0;
            std::istringstream iss(original.substr(i + 1, 2));
            if (iss >> std::hex >> converted) {
                result += static_cast<char>(converted);
                i += 2;
            } else { 
                return false;
            }            
        }

        // another way for having a white space
        else if (original[i] == '+') {
            result += ' ';
        }

        else {
            result += original[i];
        }
    }

    // successful clean up
    return true;
}


int base_request_handler::total_handler(){
    return (num_not_found_ + num_status_ + num_echo_ + num_static_ +
            num_proxy_ + num_health_ + num_itwc_);
}

bool base_request_handler::inc_number_of_handlers(std::string type_of_handler){
    mutex_.lock();
    if(type_of_handler == "echo")
        num_echo_++;
    else if(type_of_handler == "static")
        num_static_++;
    else if(type_of_handler == "404")
        num_not_found_++;
    else if(type_of_handler == "status")
        num_status_++;
    else if(type_of_handler == "proxy")
        num_proxy_++;
    else if (type_of_handler == "health")
        num_health_++;
    else if (type_of_handler == "ITWC")
        num_itwc_++;
    else {
        handle_log(1, "No increment of number of handler");
        mutex_.unlock();
        return false;
    }
    handle_log(1, "Incremented " + type_of_handler + " number of handler");
    mutex_.unlock();
    return true;
}
int base_request_handler::get_number_of_type(std::string type_of_handler) {
    if(type_of_handler == "echo") 
        return num_echo_;
    else if(type_of_handler == "static") 
        return num_static_;
    else if(type_of_handler == "404") 
        return num_not_found_;
    else if(type_of_handler == "status") 
        return num_status_;
    else if(type_of_handler == "proxy")
        return num_proxy_;
    else if (type_of_handler == "health")
        return num_health_;
    else if (type_of_handler == "ITWC")
        return num_itwc_;
    else
        return -1;
}

std::map<std::string, int> base_request_handler::get_response_code_map() {
    return response_code_map_;
}

void base_request_handler::add_to_code_map(std::string response_code) {
    mutex_.lock();
    if(response_code_map_.find(response_code) == response_code_map_.end()) { //if no match with keys
        response_code_map_.insert(std::pair<std::string, int>(response_code, 1));
        handle_log(1, "Added " + response_code + " to the code map");
        mutex_.unlock();
    }
    else { //if key already exists
        handle_log(1, "incremented " + response_code + " in the code map");
        response_code_map_[response_code] += 1;
        mutex_.unlock();
    }
}

std::map<std::string, int> base_request_handler::get_number_of_url() {
    return url_map_;
}

void base_request_handler::add_to_url_map(std::string url) {
    mutex_.lock();
    if(url_map_.find(url) == url_map_.end()) {
        handle_log(1, "Added " + url + " to url map");
        url_map_.insert(std::pair<std::string, int>(url, 1));
        mutex_.unlock();
    }
    else {
        handle_log(1, "incremented " + url + " in the url map");
        url_map_[url] += 1;
        mutex_.unlock();
    }
}

std::map<std::string,std::vector<std::string>> base_request_handler::get_all_handlers() {
    return all_handlers_;
}

void base_request_handler::add_to_all_handlers(std::string handler_name, std::string path_name) {
    if(all_handlers_.find(handler_name) == all_handlers_.end()) {
        std::vector<std::string> temp;
        temp.push_back(path_name);
        all_handlers_.insert(std::pair<std::string, std::vector<std::string>>(handler_name, temp));
    }
    else {
        all_handlers_[handler_name].push_back(path_name);
    }
}

std::string base_request_handler::get_root_path()
{ return root_path_; }
