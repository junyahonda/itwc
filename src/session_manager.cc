#include "session_manager.h"
#include "log_handler.h"

session_manager::session_manager() { }


void session_manager::start(session_ptr s) {
	mutex_.lock();
    sessions_.insert(s);
    handle_log(2, "Start session");
    mutex_.unlock();
    s->start();
    
}


void session_manager::stop(session_ptr s) {
    sessions_.erase(s);
    s->stop();
}


void session_manager::stop_all() {
    for (auto s : sessions_) {
        s->stop();
    }
    sessions_.clear();
}