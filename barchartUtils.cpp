
//
//  barchartUtils.cpp
//  Scooper
//
//  Created by Jean-Luc on 7/15/17.
//  Copyright © 2017 Jean-Luc. All rights reserved.
//

#include <libxml/tree.h>
#include <libxml/HTMLparser.h>
#include <libxml/parser.h>
#include <libxml/xmlmemory.h>
#include "barchartUtils.hpp"



// get the string from a char *
std::string get_string_from_charptr(xmlChar * char_ptr)
{
    std::string string_val = "";

    while (char_ptr){
        string_val += *char_ptr;
        char_ptr++;
    }

    return string_val;
};




// get the value of the Node Attribute
std::string get_array_attribute(std::map<std::string, std::string>& arr, std::string symbol)
{
    std::string attribute_value="";


    if(!arr.empty()){
        auto attribute = arr.find(symbol);

        if(attribute != arr.end()) {
            //std::cout << "Found " << attribute->first << " " << attribute->second << '\n';
            attribute_value = attribute->second;
        }
        else {
            std::cout << "Not found\n";
            attribute_value = "";
        }


        // iterate contentof arr
//        for (std::map<std::string,std::string>::iterator it=arr.begin(); it!=arr.end(); ++it)
//        {
//            std::cout << it->first << " => " << it->second << '\n';
//        }

    }

    return attribute_value;
};








// Give the session code according to the day of the month
std::string session_day_code(int day_of_month)
{
    std::string session_code;

    switch (day_of_month)
    {
        case 1:session_code="1";
        case 2:session_code="2";
        case 3:session_code="3";
        case 4:session_code="4";
        case 5:session_code="5";
        case 6:session_code="6";
        case 7:session_code="7";
        case 8:session_code="8";
        case 9:session_code="9";
        case 10:session_code="0";
        case 11:session_code="A";
        case 12:session_code="B";
        case 13:session_code="C";
        case 14:session_code="D";
        case 15:session_code="E";
        case 16:session_code="F";
        case 17:session_code="G";
        case 18:session_code="H";
        case 19:session_code="I";
        case 20:session_code="J";
        case 21:session_code="K";
        case 22:session_code="L";
        case 23:session_code="M";
        case 24:session_code="N";
        case 25:session_code="O";
        case 26:session_code="P";
        case 27:session_code="Q";
        case 28:session_code="R";
        case 29:session_code="S";
        case 30:session_code="T";
        case 31:session_code="U";
    }

    return session_code;

}


double apply_base_code(int price, std::string base_code)
{
    double list_price = price;
    double based_price = 0;
//    std::string bcode = base_code;

    if (price > 0)
    {
        if (base_code == "2")
        {     //  = 1/8 = one digit fraction, range is 0 - 7
                based_price = (list_price / 8);
        }
        else if (base_code == "3")
        {//  = 1/16 = two digit fraction, range is 0 - 15
                based_price = (list_price / 16);
        }
        else if (base_code == "4")
        {                //  = 1/32 = two digit fraction, range is 0 - 31
                based_price = (list_price / 32);
        }
        else if (base_code == "5")
        {                //  = 1/64 = two digit fraction, range is 0 - 63
                based_price = (list_price / 64);
        }
        else if (base_code == "6")
        {                //  = 1/128 = three digit fraction, range is 0 - 127
                based_price = (list_price / 128);
        }
        else if (base_code ==  "7")
        {                //  = 1/256 = three digit fraction, range is 0 - 255
                based_price = (list_price / 256);
                //  case "8"  //  = 0 = no decimal places, a whole number
                //  based_price = (price
        }
        else if (base_code ==  "9")
        {                //  = 0.1 = one decimal place
                based_price = (list_price / 10);
        }
        else if (base_code == "A")
        {                //  = 0.01 = two decimal places
                based_price = (list_price / 100);
        }
        else if (base_code == "B")
        {               //  = 0.001 = three decimal places
                based_price = (list_price / 1000);
        }
        else if (base_code == "C")
        {                //  = 0.0001 = four decimal places
                based_price = (list_price / 10000);
        }
        else if (base_code == "D")
        {                //  = 0.00001 = five decimal places
                based_price = (list_price / 100000);
        }
        else if (base_code == "E")
        {                //  = 0.000001 = six decimal places
                based_price = (list_price / 1000000);
        }
        else if (base_code == "F")
        {                //  = 0.0000001 = seven decimal places
                based_price = (list_price / 10000000);
        }
    }else{
        return 0;
    }


    //  puts "Applied base code for #{price} =>  #{based_price}"

    return based_price;


}
