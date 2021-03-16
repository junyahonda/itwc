#include "log_handler.h"
#include <iostream>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/chrono.hpp>

namespace logging = boost::log;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;

boost::mutex mutex_;

// takes no parameter. 
// set up the log so that when log messages are made, it is written to both file and console.
bool setup_log()
{
    std::string filename = "%m-%d-%Y_%N.log";
    filename = "../logs/" + filename; //adding path

    logging::add_file_log
    (
        
        keywords::file_name = filename,                                        /*< file name pattern >*/
        keywords::rotation_size = 10 * 1024 * 1024,                                   /*< rotate files every 10 MiB... >*/
        keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0), /*< ...or at midnight >*/
        keywords::format = //"[%TimeStamp%]: %Message%"                                 /*< log record format >*/
        (
            expr::stream
                << "[" << expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S UTC") << "] "
                << "[" << expr::attr<boost::log::attributes::current_thread_id::value_type >("ThreadID") << "] "
                << "<" << logging::trivial::severity << "> : " 
                << expr::smessage
        ),
        keywords::auto_flush = true
    );

    logging::add_console_log
    (
        std::clog,
        keywords::format = 
        (
            expr::stream
                << "[" << expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S UTC") << "] "
                << "[" << expr::attr<boost::log::attributes::current_thread_id::value_type >("ThreadID") << "] "
                << "<" << logging::trivial::severity << "> : " 
                << expr::smessage
        )
    );

    return true;
}

// Takes no parameters.
// this gets called in server_main.cc. Sets up the log file,
// then starts logging onto it after.
bool init_log() {
    if(setup_log()) {
        logging::add_common_attributes(); // necessary for date and thread ID

        using namespace logging::trivial;
        src::severity_logger< severity_level > lg;

        return true;    
    }

    else return false;
}


// handles trivial log. Takes in log_type integer and message to be used.
// log_type ints: 0 = trace, 1 = debug, 2 = info, 3 = warning, 4 = error, 5 = fatal

//sample usage: MORE ROBUST FORM 
// handle_log(0, "sample trace log");
// handle_log(1, "sample debug log");
// handle_log(2, "sample info log");   i.e. client disconnect
// handle_log(3, "sample warning log");  i.e. 
// handle_log(4, "sample error log");   i.e. Bad request
// handle_log(5, "sample fatal log");

// OR

// WARNING: LESS ROBUST, USE THE ABOVE ONES IF POSSIBLE
// BOOST_LOG_SEV(lg, trace) << "A trace severity message";
// BOOST_LOG_SEV(lg, debug) << "A debug severity message";
// BOOST_LOG_SEV(lg, info) << "An informational severity message";
// BOOST_LOG_SEV(lg, warning) << "A warning severity message";
// BOOST_LOG_SEV(lg, error) << "An error severity message";
// BOOST_LOG_SEV(lg, fatal) << "A fatal severity message";

bool handle_log(int log_type, std::string message) {
    mutex_.lock();
    switch(log_type) {
        case 0:
            BOOST_LOG_TRIVIAL(trace) << message;
            mutex_.unlock();
            return true;
        case 1:
            BOOST_LOG_TRIVIAL(debug) << message;
            mutex_.unlock();
            return true;
        case 2:
            BOOST_LOG_TRIVIAL(info) << message;
            mutex_.unlock();
            return true;
        case 3:
            BOOST_LOG_TRIVIAL(warning) << message;
            mutex_.unlock();
            return true;
        case 4:
            BOOST_LOG_TRIVIAL(error) << message;
            mutex_.unlock();
            return true;
        case 5:
            BOOST_LOG_TRIVIAL(fatal) << message;
            mutex_.unlock();
            return true;
        default:
            BOOST_LOG_TRIVIAL(error) << "Logging error, log_type was not a valid number.";
            mutex_.unlock();
            return false;
    }
}


bool handle_machine_log(log_data &tag, int log_type, std::string message) {
    std::string tags = " [ResponseMetrics]";
    tags +=            " (1)RESPONSE CODE: " + tag.response_code;
    tags +=            ", (2)REQUEST HANDLER: " + tag.request_handler;
    tags +=            ", (3)IP_ADDRESS: " + tag.ip_address;
    message += tags;
    // to remove dependency, it would be better to call these 
    // instead of calling handle_log.
    // when called handle_log(log_type,message) 
    //          -> log_handler coverage : 76.9%
    // when using the below 
    //          -> log_handler coverage : 99%
    mutex_.lock();
    switch(log_type) {
        case 0:
            BOOST_LOG_TRIVIAL(trace) << message;
            mutex_.unlock();
            return true;
        case 1:
            BOOST_LOG_TRIVIAL(debug) << message;
            mutex_.unlock();
            return true;
        case 2:
            BOOST_LOG_TRIVIAL(info) << message;
            mutex_.unlock();
            return true;
        case 3:
            BOOST_LOG_TRIVIAL(warning) << message;
            mutex_.unlock();
            return true;
        case 4:
            BOOST_LOG_TRIVIAL(error) << message;
            mutex_.unlock();
            return true;
        case 5:
            BOOST_LOG_TRIVIAL(fatal) << message;
            mutex_.unlock();
            return true;
        default:
            BOOST_LOG_TRIVIAL(error) << "Logging error, log_type was not a valid number.";
            mutex_.unlock();
            return false;
    }
}
