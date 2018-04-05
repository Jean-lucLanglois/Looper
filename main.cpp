//
//  main.cpp
//  scooper
//
//  Created by Jean-Luc Langlois on 6/27/2017.
//  Copyright © 2017 Momentum Leads. All rights reserved.
//
/*

  /|      /||\\   /||\\   /||\\   |||||   |||\\
  ||      || ||   || ||   ||  |   ||      ||  |
  ||      || ||   || ||   |||//   |||     |||//
  ||      || ||   || ||   ||      ||      ||\\
  \\|//   \\|//   \\|//   ||      |||||   || \\

 scooper engine for linux >
 - Check Engine status in DB
 - Connect & Retrieve Stock List from DB
 - Connect & Retrieve data from provider
 - Parse data & extrapulate values
 - Process algorytmic sequences
 - EMA12()
 - EMA26()
 - MACD()
 - MSignal()
 - Histogram()
 - Update dabase server

 //     Libraries for algorythmic sequences
 //  Eigen
 //  boost (the mathematical part)
 //  glpk
 //  gsl

 */


#include <string>
#include <iostream>
#include <pqxx/pqxx>
#include <unistd.h>
#include <algorithm>
#include <thread>
#include <chrono>
#include <ctime>
#include "db.hpp"
#include "Grabber.hpp"
#include "time_support.hpp"


const int THROTTLE = 50;
const int DURATION = 300; // seconds
const char * BASE_URL = "http://qs-us-e-01.aws.barchart.com/stream/quotes.jsx?username=galuga&password=pepe7373&symbols=";

// Function isEven - Find even number true/false
bool isEven(int num){
    return num % 2;
}

// Main
int main()
{

    // Log the start of scooper and defaults
    std::cout << "Starting ScOOper.. v1.269" << std::endl;
    std::cout << "Server engine stats -> " << server_started() << std::endl;
    std::cout << "THROTTLE -> " << THROTTLE << std::endl;
    std::cout << "DURATION -> " << DURATION << " seconds" << std::endl;

    // Var declaration
    int sleep_interuption = 0;

    // Let's do this every interval of time
    while(DURATION > 0) // infinite loop !!TODO!!
    {
        try{

            // let's make sure the database server status is open and the server property is on
            while(server_started())
            {
                 //  Let's get a time stamp at the beggining.
                auto start_scooping = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
                std::cout << get_current_time() << std::endl;

                try //  Check if server is up
                {
                    //  Get stocks list from postgres
                    pqxx::result r = query_stocks();
                    std::vector<std::string>stock_names;
    //                int stock_name_count = 0;               // Records count received from server to query
                    // int queried_records = 0;             // Total record passed to barchart for query
                    int qcount = 0;                         // Count how many records to query

                    // Build the URL
                    std::string url_origin = BASE_URL;
                    std::string url_tail = url_origin;

                    // Iterate over the rows in our result set.  Results objects
                    // are containers similar to std::vector and such.

                    for (pqxx::result::const_iterator row = r.begin();
                         row != r.end();
                         ++row)
                    {
                        qcount += 1;                        // increment quantity count qcount as to throttle
    //                    std::cout << "qcount : " << qcount << std::endl;

                        if(row.size() > 0){                // Let's make sure we have data
                            if( qcount > 1){                    // Let's add a comma after the first symbol
                                url_tail = url_tail + ",";
                            }
                            url_tail = url_tail + row["stock_name"].as<std::string>();
    //                        std::cout <<  "\u2713";
                            //std::cout << "Url Tail Symbol: " << row["stock_name"].as<std::string>();
                            //std::cout << " THROTTLE Index: " << qcount << ":" << r.size() << std::endl;
                        }


                        // Let's create a Grabber Object if at Throttle
                        if (qcount >= THROTTLE){
                            // Let's build it and pull server session,
                            // then reset and continue looping through set
                            std::cout << "(" << qcount << ") " ;
                            Grabber grab(url_tail);
                            grab.pull_server_session();
                            url_origin = BASE_URL;
                            url_tail = BASE_URL;
                            qcount =0;

                        }
                    }

                    // If we build a string of stock names but never reached THROTTLE
                    // Lets call Grabber on that url
                    if (qcount > 0){
                        // Let's build it and pull server session,
                        // then reset and continue looping through set
                        std::cout << "(" << qcount << ") " ;
                        Grabber grab(url_tail);
                        grab.pull_server_session();
                        url_origin = BASE_URL;
                        url_tail = BASE_URL;
                        qcount =0;

                    }

                    // Let's get a time stamp and get milliseconds since epoch
                    auto end_scooping  = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
                    auto start_since_epoc  = start_scooping.time_since_epoch();
                    auto end_since_epoc = end_scooping.time_since_epoch();
                    auto start_epoc_ms = std::chrono::duration_cast<std::chrono::milliseconds>(start_since_epoc);
                    auto end_epoc_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_since_epoc);

                    // let's get how many milliseconds since the program started
                    long duration_ms = end_epoc_ms.count() - start_epoc_ms.count();
                    std::cout << "Processing Time " << duration_ms << "ms" << std::endl;

                    // Let's get the thread to sleep for the remaining 5 minutes
                    auto sleepy_time = 300000 - duration_ms; // 300 seconds or 5 minutes
                    std::cout << "Sleeping for " << sleepy_time << "ms" << std::endl;
                    std::this_thread::sleep_for(std::chrono::milliseconds(sleepy_time));

                }
                catch (const std::exception& e)
                {
                    std::cerr << "Error: " << e.what() << std::endl;
                    return 1;
                }
                catch (const pqxx::sql_error& e)
                {
                    std::cerr << "SQL error: " << e.what() << std::endl;
                    std::cerr << "Query was: " << e.query() << std::endl;
                    return 2;
                }

            }; // while (server_started)

            // The server stopped
            // Let's sleep the thread for 10 seconds
            sleep_interuption++;
            std::this_thread::sleep_for(std::chrono::milliseconds(10000));
            std::cout << "Sleeping for 10 seconds.(" << sleep_interuption * 10 << "s)" << std::endl;

        }
        catch (const std::exception& e)
        {
            std::cerr << "Error: " << e.what() << std::endl;
            return 1;
        }
        catch (const pqxx::sql_error& e)
        {
            std::cerr << "SQL error: " << e.what() << std::endl;
            std::cerr << "Query was: " << e.query() << std::endl;
            return 2;
        }
    }; // while DURATION

    return 0;
}




