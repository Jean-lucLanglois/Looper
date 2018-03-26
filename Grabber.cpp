//
//  GRABBER.cpp

//  Scooper
//
//  Created by Jean-Luc on 7/12/17.
//  Copyright © 2017 Jean-Luc. All rights reserved.
//


#include <pqxx/pqxx>
#include <libxml/tree.h>
#include <libxml/HTMLparser.h>
#include <libxml/parser.h>
#include <libxml/xmlmemory.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include "Grabber.hpp"
#include "barchartUtils.hpp"
#include "db.hpp"
#include "time_support.hpp"

#define HEADER_ACCEPT "Accept:text/html,application/xhtml+xml,application/xml"
#define HEADER_USER_AGENT "User-Agent:Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.17 (KHTML, like Gecko) Chrome/24.0.1312.70 Safari/537.17"

// CLASS GRABBER INITIALISER
Grabber::Grabber(std::string &query_string){
    // Simple test to validate the url starts with http before
    if (query_string[0] == 'h' && query_string[1] == 't' && query_string[2] == 't' && query_string[3] == 'p')
        this->url = query_string;
}



// CLASS GRABBER PULL_SERVER_SESSION
bool Grabber::pull_server_session(){

    // Variables needed for calculations
    //std::string stock_name, stock_symbol, base_code, ask_size, volume, numtrades;
    //std::string ask, data_mode, price_volume, trade_size, trade_time, quote_last_updated;
    //std::string session_session, session_ticks;

    // Establish DB Connection
//    pqxx::connection c{"dbname=instigator user=jean-luc password=ju_vx014_01"};
//    pqxx::work txn{c};

    std::cout << " <<< Grabber :: Pulling Barchart Data" << std::endl;
    // std::cout << url << std::endl;
    // std::string url = "http://qs-us-e-01.aws.barchart.com/stream/quotes.jsx?username=galuga&password=pepe7373&symbols=AAPL,GOOGL";

    /* Macro to check API for match with the DLL we are using */
    // LIBXML_TEST_VERSION

    //  CURL --  Specify the URL
    request.setOpt(curlpp::options::Url(url));

    // Specify some headers options
    std::list<std::string> headers;
    headers.push_back(HEADER_ACCEPT);
    headers.push_back(HEADER_USER_AGENT);
    request.setOpt(new curlpp::options::HttpHeader(headers));
    request.setOpt(new curlpp::options::FollowLocation(true));

    // Configure curlpp to use stream
    std::ostringstream responseStream;
    curlpp::options::WriteStream streamWriter(&responseStream);
    request.setOpt(streamWriter);

    // Collect response
    request.perform();
    std::string re = responseStream.str();
    //    std::cout << re << std::endl;

    // Objects holding data
    std::map<std::string, std::string> quote_array = {};
    std::map<std::string, std::string> session_array = {};
    std::map<std::string,std::string>::iterator it;


    // XML PARSER
    // Parse HTML and create a DOM tree
    xmlDoc* doc = htmlReadDoc((xmlChar*)re.c_str(), NULL, NULL, HTML_PARSE_RECOVER | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING);
    //    xmlDoc* doc = xmlParseFile((xmlChar*)re.c_str(), NULL, NULL, HTML_PARSE_RECOVER | HTML_PARSE_NOERROR | HTML_PARSE_NOWARNING);
    // Encapsulate raw libxml document in a libxml++ wrapper
    xmlNode * root_element = xmlDocGetRootElement(doc);
    //    print_element_names(root_node);


    // Exit if root_element is null
    if (root_element == NULL)
    {
        fprintf(stderr, "empty document\n");
        xmlFreeDoc(doc);
        return 0;
    }

    // Let's iterate through the root_element Node
    for ( auto cur_node = root_element; cur_node; cur_node = cur_node->next)
    {
        // Test to get only the XML_ELEMENT_NODE
        if (cur_node->type == XML_ELEMENT_NODE)
        {
            /* Check for if current node should be exclude or not */
//            printf("Node type: Text, name: %s\n", cur_node->name);

            // Let's dig in a couple of levels to the quotes
            cur_node = cur_node->children;
            cur_node = cur_node->children;

            // Couldn't find what we wanted. Let's bail!
            if(NULL == cur_node)
            {
                printf("Invalid Node %p\n", cur_node);
                return 1;
            }

            // Let's iterate the quote elements
            // xmlNode *cur_quote = NULL;
            for (xmlNode *cur_quote = cur_node->children; cur_quote; cur_quote = cur_quote->next) {

                // Let's check for the type
                if (cur_quote->type == XML_ELEMENT_NODE) {
//                    printf("-------------------------  %s\n", cur_quote->name);
                    // printf("\t\tContent: %s\n", xmlNodeGetContent(cur_quote));

                    // Let's get the attributes from the quote
                    xmlAttr* quote_property = cur_quote->properties;
                    while(quote_property)
                    {
                        // Parse quote attribute (qattribute) and convert to string
                        std::string quote_attribute_name = reinterpret_cast<const char*>(quote_property->name);
                        xmlChar* quote_attribute_value = xmlNodeListGetString(cur_node->doc, quote_property->children, 1);
                        std::string qattrib_val(reinterpret_cast<char*>(quote_attribute_value));
                        //printf("\t\%s: %s\n", quote_property->name, quote_attribute_value );

                        // Add it to the map array
//                        quote_array.insert(quote_attribute_name, get_string_from_charptr(quote_attribute_value));
                        quote_array[quote_attribute_name] = qattrib_val;

                        // Let's free up memeory and move to next record
                        xmlFree(quote_attribute_value);
                        quote_property = quote_property->next;
                    }

                    // Let's iterate the session elements // children of quotes
                    xmlNode *cur_sess = NULL;
                    for(cur_sess = cur_quote->children; cur_sess; cur_sess = cur_sess->next){

                        // Let's check for the type
                        if (cur_sess->type == XML_ELEMENT_NODE){
                            //printf("\t------------------------- %s\n", cur_sess->name);

                            // Let's check for the id type. We're looking for the combined value
                            xmlChar *id = xmlGetProp(cur_sess, (const xmlChar *)"id");
                            std::string id_val(reinterpret_cast<char*>(id));
                            if(id_val == "combined")
                                {
                                // checking the correct id of combined
                                printf("id: %s\n",id);

                                // Let's get the attributes from the session
                                xmlAttr* session_property = cur_sess->properties;
                                while(session_property)
                                {
                                    // Let's get the name of the attribute
                                    std::string session_attribute_name = (char *)session_property->name ;
                                    //reinterpret_cast<const char*>(session_property->name);
                                    xmlChar* session_attribute_value = xmlNodeListGetString(cur_node->doc, session_property->children, 1);

                                    // Let's get the value of the attribute
                                    std::string sattrib_val(reinterpret_cast<char*>(session_attribute_value));
                                    //printf("\t\%s: %s\n", session_property->name, session_attribute_value );

                                    // Let's add the pair to the array
                                    session_array[session_attribute_name] = sattrib_val;

                                    // Let's free up memory and move next
                                    xmlFree(session_attribute_value);
                                    session_property = session_property->next;
                                }
                                std::cout << "End of Session ----------------------\n\n" << std::endl;

                                // Let's get the values we need
                                // Stock Name
                                std::string stock_name = get_array_attribute(quote_array,  "symbol");

                                // Base code
                                std::string basecode = get_array_attribute(quote_array, "basecode");

                                // Open_price
                                //std::string open_price = get_array_attribute(session_array, "open");
                                //if(open_price == ""){ open_price = "0";};
                                //double open_based_price = apply_base_code(std::stoi(open_price), basecode);

                                // high_price
                                //std::string high_price = get_array_attribute(session_array, "high");
                                //if(high_price == ""){ high_price = "0";};
                                //double high_based_price = apply_base_code(std::stoi(high_price), basecode);

                                // Low_price
                                //std::string low_price = get_array_attribute(session_array, "low");
                                //if(low_price == ""){ low_price = "0";};
                                //double low_based_price = apply_base_code(std::stoi(low_price), basecode);

                                // Last_price
                                std::string last_price = get_array_attribute(session_array, "last");
                                if(last_price == ""){ last_price = "0";};
                                double last_based_price = apply_base_code(std::stoi(last_price), basecode);

                                // Ask price ==> based_price
                                std::string ask_price = get_array_attribute(quote_array, "ask");
                                if(ask_price == ""){ ask_price = last_price;};
                                if(ask_price == ""){ ask_price = "0";};
                                double ask_based_price = apply_base_code(std::stoi(ask_price), basecode);

                                // Previous Price
                                //std::string previous_price = get_array_attribute(quote_array, "previous");
                                //if (previous_price == ""){previous_price = "0";};
                                //double previous_based_price = apply_base_code(std::stoi(previous_price), basecode);

                                // Bid price
                                std::string bid_price = get_array_attribute(quote_array, "bid");
                                if(bid_price == ""){ bid_price = "0";};
                                double bid_based_price = apply_base_code(std::stoi(bid_price), basecode);

                                // Volume
                                std::string volume = get_array_attribute(session_array,"volume");
                                if (volume == ""){volume = "0";};

                                // Price Volume
                                std::string pricevolume = get_array_attribute(session_array,"pricevolume");
                                if (pricevolume == ""){pricevolume = "0";};

                                // Trade Size
                                std::string tradesize = get_array_attribute(session_array,"tradesize");
                                if (tradesize == ""){tradesize = "0";};

                                // Number trades
                                std::string numtrades = get_array_attribute(session_array,"numtrades");
                                if (numtrades == ""){numtrades = "0";};

                                //
                                std::cout << "Stock Name: " << stock_name << std::endl;
                                std::cout << "Ask price: " << ask_based_price << std::endl;
                                std::cout << "Bid price: " << bid_based_price << std::endl;
                                std::cout << "Price Volume: " << pricevolume << std::endl;

                                // Let's add the record to the sql server
                                // Create SQL statement
                                std::string sql = "INSERT INTO digger_stocks("  \
                                "stock_name, volume, price_volume, trade_size, num_trades, ask, bid, rating, created_at, updated_at)"\
                                "VALUES('"\
                                + stock_name + "','" \
                                + volume + "','" \
                                + pricevolume + "','" \
                                + tradesize + "','" \
                                + numtrades + "','" \
                                + std::to_string(ask_based_price) + "','" \
                                + std::to_string(bid_based_price) + "','" \
                                "1','" \
                                + currentDateTime() + "','" \
                                + currentDateTime() + "');";
                                //std::cout << "SQL:" << sql << std::endl;


                                /* Execute SQL query */
                                if(ask_based_price > 0){
                                    insert_digger_stock(sql);
        //                                    std::cout << "Created successfully" << std::endl;
                                }else{
                                    std::cout << "Did not add record to DB.\n empty 'Ask Based Price' present." << std::endl;

                                }
                            }
    //                            }

                        // Let's null the two arrays
    //                            quote_array = {};
    //                            session_array = {};
    //                        }
                            }
                        }
                    }
            }
        }
        else if(cur_node->type == XML_TEXT_NODE)
        {
            /* Process here text node, It is available in cpStr :TODO: */
            // printf("node type: Text, node content: %s,  content length %zu\n", (char *)cur_node->content, strlen((char *)cur_node->content));
        }
    }

    xmlFreeDoc(doc);       // free document
    xmlCleanupParser();    // Free globals

    //// std::cout << " SESSION #" << session_session;
    //// std::cout << std::setw(11) << std::left << "  Volume: " << volume;
    //// std::cout << std::setw(8) << std::left << "  Ask: " << ask;
    //// std::cout << std::setw(15) << std::left << "  Price Volume: " << price_volume;
    //// std::cout << std::setw(8) << std::left << "  Trade Size: " << trade_size;
    //// std::cout << std::setw(16) << std::left << "  Trade Time: " << trade_time;
    //// std::cout << std::setw(8) << std::left << "  Numtrades: " << numtrades;
    //// std::cout << std::setw(5) << std::left << "  Session Ticks: " << session_ticks << std::endl;

    return true;
}


// Deconstructor
Grabber::~Grabber(){};

