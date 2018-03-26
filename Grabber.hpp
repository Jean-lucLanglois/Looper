//
//  Grabber.hpp
//  Scooper
//
//  Created by Jean-Luc on 7/12/17.
//  Copyright © 2017 Jean-Luc. All rights reserved.
//

#ifndef Grabber_hpp
#define Grabber_hpp

#include <string>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

// CLASS GRABBER
class Grabber {

public:
    Grabber();
    Grabber(std::string& query_string);
    bool pull_server_session();
    ~Grabber();

private:
    std::string url;
    curlpp::Easy request;
    std::map<const std::string, std::string> sessionAttribs;
    std::map<const char*, char*> quoteAttribs;

};

#endif /* grabber_hpp */

