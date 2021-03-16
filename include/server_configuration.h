#ifndef SERVER_CONFIG
#define SERVER_CONFIG

#include <iostream>
#include "config_parser.h"
#include "data_structures.h"
#include <map>
#include <vector>

class server_configuration{
public:
	server_configuration();

	// Argument : port number
	// Output   : -
	// setting port number with the argument integer port 
	void set_port(int port);

	// Argument : -
	// Output   : port number
	// return the stored port number from private
	int get_port();

	// Argument : -
	// Output   : port number
	// return the handler_config_vector_
	std::vector<handler_config> get_handler_vec();

	// Argument : -
	// Output   : map
	std::map<std::string, std::string> get_path();


	// Argument : (string)type, (string)Path
	// Output   : -
	// create an element in the map that connects type to path 
	void set_path(std::string type, std::string path);

	// Argument : NginxConfig
	// Output   : - 
	// the function is working with map_configuration
	// it helps map_statement receive the next child.
	void map_configuration(NginxConfig *conf);

	// Argument : NginxConfigStatement
	// Output : true or false
	// the function is working with map_configuration
	// map_statement is checking the current tokens.
	// it takes statement, and check token by token in config file
	// saving the important data (path or port number)
	bool map_statement(NginxConfigStatement *statement);



private:
	std::map<std::string, std::string> map_path_;
	std::vector<handler_config> handler_config_vector_;
	int port_num_;
};





#endif