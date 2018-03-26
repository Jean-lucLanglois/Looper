
//
//  barchartUtils.hpp
//  Scooper
//
//  Created by Jean-Luc on 7/15/17.
//  Copyright © 2017 Jean-Luc. All rights reserved.
//

#ifndef barchartUtils_hpp
#define barchartUtils_hpp

#include <stdio.h>
#include <iostream>
#include <map>



// get the string from a char *
std::string get_string_from_charptr(xmlChar * char_ptr);

// get the value of the Node Attribute
std::string get_array_attribute(std::map<std::string, std::string>& arr, std::string symbol);

// Give the session code according to the day of the month
std::string session_day_code(int day_of_month);

// Get base code from price and base_code
double apply_base_code(int price, std::string base_code);

#endif /* barchartUtils_hpp */
