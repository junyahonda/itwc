#ifndef SESSION_MANAGER_H
#define SESSION_MANAGER_H

#include <set>
#include "session.h"
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/chrono.hpp>

class session_manager {
public:
    // disallow copy/assignment constructors
    session_manager(const session_manager&) = delete;
    session_manager& operator=(const session_manager&) = delete;


    // Constructor
    //   Arguments: None  
    //   Output: Initialize
    session_manager();


    // start
    //   Arguments: s (session_ptr)
    //   Output: None
    // add s to sessions_ and start the s
    void start(session_ptr s);


    // stop
    //   Arguments: s (session_ptr)
    //   Output: None
    // remove s from sessions_ and stop the s
    void stop(session_ptr s);


    // stop_all
    //   Arguments: None
    //   Output: None
    // iterate through all session_ptr in sessions_
    // and stop the session. then clear them
    void stop_all();

private:
    // currently managing sessions
    std::set<session_ptr> sessions_;
    boost::mutex mutex_;
};




#endif // SESSION_MANAGER_H