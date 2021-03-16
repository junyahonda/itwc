#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <boost/asio.hpp>
#include "server.h"
#include "config_parser.h"
#include "log_handler.h"
#include "server_configuration.h"
#include "data_structures.h"

int main(int argc, char* argv[]) {
    // try to run the server first
    try {
        // when running the server, it requires an arguments of config_file
        if (argc != 2) {
            handle_log(5, "Usage: server <config_file>");
            return 1;            
        }

        // initialize logging
        init_log();

        NginxConfigParser parser;
        NginxConfig out_config;

        handle_log(2, "Parsing configuration file...");
        // parse and get tokens in the config file.
        parser.Parse(argv[1], &out_config);
        std::cout << out_config.ToString() << std::endl;
        
        // The server configuration is for storing the information:
        // port number, static path or echo path, and so on.
        server_configuration server_config;

        // The data has been recorded after passing through map_configuration
        server_config.map_configuration(&out_config);

        // the port number has been saved after map_configuration. the port number can be called.
        handle_log(2, "Parsing completed, starting server at port number " + std::to_string(server_config.get_port()));
        
        // important data is all stored into the map from config file.
        // std::map<std::string, std::string> map_setting = server_config.get_path();
        // for (auto it = map_setting.begin(); it != map_setting.end(); ++it)
        //     std::cout << it->first << ", " << it->second << std::endl;

        // // temporary handler_config vector so we can test the functions in handle_dispatch
        // std::vector<handler_config> temp_config;
        // handler_config config1 = {handler_config::Echo, "/echo", "../data/echo_data"};
        // handler_config config2 = {handler_config::Static, "/static", "../data/static_data"};
        // temp_config.push_back(config1);
        // temp_config.push_back(config2);

        // for (auto it = temp_config.begin(); it != temp_config.end(); it++) {
        //     std::cout << it->type << " " << it->url << " " << it->path << std::endl;
        // }


        // initialize the server 
        server s("127.0.0.1", std::to_string(server_config.get_port()), server_config.get_handler_vec());
        handle_log(2, "Server has been started.");

        
        // handle_log(2, "Initialized handlers")
        s.run();
    }
    catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        handle_log(5, "Exception occured. Could not start the server.");
    }
}

