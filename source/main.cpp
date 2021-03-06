//
// Copyright (c) 2017, ivmeroLabs.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this 
// software and associated documentation files (the "Software"), to deal in the Software
// without restriction, including without limitation the rights to use, copy, modify, 
// merge, publish, distribute, sublicense, and/or sell copies of the Software, and to 
// permit persons to whom the Software is furnished to do so, subject to the following 
// conditions:

// The above copyright notice and this permission notice shall be included in all copies 
// or substantial portions of the Software. 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE 
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
// TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE 
// USE OR OTHER DEALINGS IN THE SOFTWARE.
//

#include <thread>
#include <iostream>

#include <mongocxx/exception/exception.hpp>
#include <bsoncxx/json.hpp>

#include "mongodb_pool_singleton.hpp"

void connectUsingPool() {
    try {
        auto conn = MongodbPoolSingleton::shared().connection().acquire();
        auto db = (*conn)[MongodbPoolSingleton::_config.database()];
        if (db.has_collection("mycoll")) {
            auto cursor = db["mycoll"].find({});
            for (auto && doc : cursor) {
                std::cout << bsoncxx::to_json(doc) << '\n';
            }
        }
    }
    catch(mongocxx::exception::system_error & e) {
        std::cerr << "connection fail: " << e.what() << '\n';
    }
}

int main(int argc, const char * argv[]) {
    try {
        MongodbPoolSingleton::_config = MongodbPoolSingletonConfiguration("config.json");

        std::thread th1(connectUsingPool);
        std::thread th2(connectUsingPool);
        std::thread th3(connectUsingPool);
        std::thread th4(connectUsingPool);

        th1.join();
        th2.join();
        th3.join();
        th4.join();
    }
    catch(std::exception & e) {
        std::cout << "error: " << e.what() << '\n';
    }
}