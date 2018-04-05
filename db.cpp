//
//  db.cpp
//  Scooper
//
//  Created by Jean-Luc on 7/1/17.
//  Copyright © 2017 Jean-Luc. All rights reserved.
//

#include <pqxx/pqxx>
#include <stdio.h>
#include <iostream>
#include "db.hpp"


//pqxx::work txn{c};
int volume_threshold =0;
double price_threshold = 0.000;


/// Query employees from database.  Return result.
bool server_started()
{
    // Establish DB Connection
    // pqxx::connection c{"dbname=instigator user=jean-luc password=ju_vx014_01"};
    pqxx::connection c{"user=instigator_app  host=jeanluc-db.c0hxc3wgxzra.us-east-1.rds.amazonaws.com  password=zfszsT38ED  dbname=instigator"};
    pqxx::work txn{c};

    pqxx::result r = txn.exec("SELECT engine_status FROM app_preferences");
    txn.commit();

    std::cout << "Engine Status:" << r[0]["engine_status"].as<bool>() << std::endl;

    // Let's return engine_status as a boolean
    return r[0]["engine_status"].as<bool>();

}


/// Query employees from database.  Return result.
pqxx::result query_stocks()
{
    // Establish DB Connection
    //pqxx::connection c{"dbname=instigator user=jean-luc password=ju_vx014_01"};
    pqxx::connection c{"user=instigator_app  host=jeanluc-db.c0hxc3wgxzra.us-east-1.rds.amazonaws.com  password=zfszsT38ED  dbname=instigator"};
    pqxx::work txn{c};

    std::cout << "Pulling stock names from postgresql " << std::endl;

    // Query server for digger stock names
    pqxx::result r = txn.exec("SELECT * FROM digger_stock_names order by stock_name asc");
    txn.commit();

    // Let's print out how many records found
    std::cout << r.size() << " stock names pulled for inquiry." << std::endl;

    // Let's return the results here and exit
    return r;
}


/// Insert row in digger_stock .  Return result.
bool insert_digger_stock(std::string sql)
{
    // Establish DB Connection
    //pqxx::connection c{"dbname=instigator user=jean-luc password=ju_vx014_01"};
    pqxx::connection c{"user=instigator_app  host=jeanluc-db.c0hxc3wgxzra.us-east-1.rds.amazonaws.com  password=zfszsT38ED  dbname=instigator"};
    pqxx::work txn{c};

    pqxx::result r = txn.exec(sql);
    txn.commit();// explicit about when the transaction is done.
    return true; // Connection object goes out of scope here.  It closes automatically.

}




