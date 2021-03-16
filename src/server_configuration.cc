#include "server_configuration.h"
#include "log_handler.h"
#include "base_request_handler.h"
#include <string>

server_configuration::server_configuration() {
    // setting default path
    map_path_["/"] = "../data";
    map_path_["/status"] = "";
    // setting default path for echo 
    map_path_["/echo"] = "../data/echo_data";
}

void server_configuration::set_port(int port) {
    port_num_ = port;
}

int server_configuration::get_port() {
    return port_num_;
}

std::map<std::string, std::string> server_configuration::get_path() {
    return map_path_;
}

std::vector<handler_config> server_configuration::get_handler_vec() {
    return handler_config_vector_;
}

void server_configuration::set_path(std::string type, std::string path) {
    map_path_[type] = path;
}

void server_configuration::map_configuration(NginxConfig *conf) {
    for (auto& statement : conf->statements_) {
        map_statement(&(*statement));
    }
}

// Set url path for token next to location token
bool server_configuration::map_statement(NginxConfigStatement *statement) {
    bool child_visited = false;
    std::cout << "CURRENT tokens[0]: " << statement->tokens_[0] << std::endl;

    // location
    if (statement->tokens_.size() > 1) {
        std::cout << "CURRENT tokens[1]: " << statement->tokens_[1] << std::endl;
        if (statement->tokens_[0] == "location") {
            if (statement->tokens_.size() > 2) {
                handler_config temp;

                std::string prefix  = statement->tokens_[1];
                prefix = prefix.substr(1, prefix.length() - 2);
                handle_log(1, "url is: " + prefix);
                if (prefix == "/") {
                    return false;
                }
                std::string handler  = statement->tokens_[2];
                handle_log(1, "handler is: " + handler);
                
                NginxConfig* config_pointer = statement->child_block_.get();
                NginxConfig config = *config_pointer;

                if (config_pointer == nullptr) {
                    handle_log(4, "Location does not exist.");
                    // block scope is expected but does not exist
                    return false;
                }

                base_request_handler::add_to_all_handlers(handler, prefix);
                if(handler == "EchoHandler") {
                    temp.type = handler_config::Echo;
                }
                else if(handler == "StaticHandler") {
                    temp.type = handler_config::Static;
                }
                else if(handler == "StatusHandler") {
                    temp.type = handler_config::Status;
                }
                else if(handler == "ReverseProxyHandler"){
                    temp.type = handler_config::Reverse_proxy;
                }
                else if(handler == "HealthHandler") {
                    temp.type = handler_config::Health;
                }
                else if(handler == "ITWCHandler") {
                    temp.type = handler_config::ITWC;
                }
                else {
                    temp.type = handler_config::Not_found;
                }

                temp.location_prefix = prefix;
                // temp.path = path;
                temp.config = config;

                handler_config_vector_.push_back(temp);
            }
            else {
                handle_log(4, "Invalid config file syntax.");
                return false;
            }
        }
        else if (statement->tokens_[0] == "port") {
            handle_log(1, "port is: " + statement->tokens_[1]);
            set_port(std::stoi(statement->tokens_[1]));
        }
    }
    if (statement->child_block_.get() != nullptr && child_visited == false) {
        map_configuration(statement->child_block_.get());
    }
    return true;
}