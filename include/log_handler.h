#include <string.h>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

#include <map>
#include <vector>
#include "data_structures.h"

// Argument : -
// Output   : bool (when it is sucessful)
// Before starting logging, it set up all setting.
// so that when log messages are made,
// it is written to both file and console.
bool setup_log();

// Argument : -
// Output   : bool (when it is sucessful)
// it gets called in server_main.cc. Sets up the log file,
// then starts logging onto it after.
bool init_log();

// Argument : (int) log_type(0~5), (string) message
// Output   : bool (when it is sucessful)
// it is the function to write the log. 
// the logging can be categorized with log_type
// and the message can be written in the message as a string.
// if the log_type in parameter is not between 0~5,
// it returns false, or else, true
bool handle_log(int log_type, std::string message);
                                                  
// Argument : log_data, (int) log_type(0~5), (string) message
// Output   : true or false.
// It is for logging of requests. 
// It creates machine parsable machine log for each request.
bool handle_machine_log(log_data &tag, int log_type, std::string message);