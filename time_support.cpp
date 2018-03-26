
//
//  time_support.cpp
//  Scooper
//
//  Created by Jean-Luc on 8/24/17.
//  Copyright © 2017 Jean-Luc. All rights reserved.
//

#include "time_support.hpp"


std::string get_current_time(){

    time_t currentTime;
    struct tm *localTime;
    std::string current_time;

    time( &currentTime );                   // Get the current time
    localTime = localtime( &currentTime );  // Convert the current time to the local time

//    int Day    = localTime->tm_mday;
//    int Month  = localTime->tm_mon + 1;
//    int Year   = localTime->tm_year + 1900;
    int Hour   = localTime->tm_hour;
    int Min    = localTime->tm_min;
    int Sec    = localTime->tm_sec;

    current_time = "This program was exectued at: " + std::to_string(Hour) + ":" + std::to_string(Min) + ":" + std::to_string(Sec) ;
//    current_time = "And the current date is: " + std::to_string(Day) + "/" + std::to_string(Month) + "/" + std::to_string(Year);
    return current_time;
};



// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d %X", &tstruct);

    return buf;
}
