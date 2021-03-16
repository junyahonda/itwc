#ifndef MIME_TYPES_H
#define MIME_TYPES_H

#include <string>



// get_ext_type
//   Arguments: ext (string&)  
//   Output: content_type (string)
// with given file extension, returns a string
// with corresponding content-type message
std::string get_ext_type(const std::string &ext);



#endif // MIME_TYPES_H
