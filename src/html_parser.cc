#include "html_parser.h"

#include <string>
#include <iostream>
#include <regex>

namespace html_parsing {
    std::string update_tag_attribute(std::string html_doc, std::string prefix, std::string attribute) {
        const std::string tag_open_regex_str = "(<[a-zA-Z]\\w*?";
        const std::string attributes_regex_str = "(\\s*\\w+\\s*=\\s*\"[^\"]*\")*";
        const std::string href_regex_str = "\\s*" + attribute + "\\s*=\\s*\")((/[^\"]*)*/?)(\"";
        const std::string tag_close_regex_str = "\\s*\\/?>)";

        // The regex used is detailed below. To recreate the tag as it was originally, the first,
        // capture group (start of tag to right before the href link), third capture group (the link), and
        // the fifth capture group (after the link until the end of the tag) are copied over to the replacement.
        // In this example, we are setting attribute to be "href".
        //(<[a-zA-Z]\w*?(\s*\w+\s*=\s*"[^"]*")*\s*href\s*=\s*")((\/[^"]*)*)("(\s*\w+\s*=\s*"[^"]*")*\s*>)
        // i.e. for <a href="/hi"/>
        // 1:      <a href="
        // prefix: /example 
        // 3:      /hi
        // 5:      "/>
        //
        // This regex will find the provided attribute in any html tags and update them if they have a relative path

        const std::string replacement_regex_str = "$1" + prefix + "$3$5";
        const std::string uri_finder = tag_open_regex_str + attributes_regex_str + href_regex_str + attributes_regex_str + tag_close_regex_str;
        const std::regex  uri_regex = std::regex(uri_finder);

        return std::regex_replace(html_doc, uri_regex, replacement_regex_str);
    }
}