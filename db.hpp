
//
//  db.hpp
//  Scooper
//
//  Created by Jean-Luc on 7/1/17.
//  Copyright © 2017 Jean-Luc. All rights reserved.
//

#ifndef db_hpp
#define db_hpp



// Prototyping
pqxx::result query_stocks();
bool insert_digger_stock(std::string sql);
bool server_started();


#endif /* db_hpp */
