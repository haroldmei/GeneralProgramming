#include <iostream>
#include <fstream>
#include <vector>

#include "rpc/client.h"
#include "./msg.hh"

using namespace std;

void worker(rpc::client *c){

    while(true){
        // Calling a function with paramters and converting the result to int
        cout << "worker started" << endl;
        mr_msg result = c->call("foo").as<mr_msg>();

        std::cout << "Type: " << result.msg_type << std::endl;
        std::cout << "Value: " << result.message << std::endl;
        std::cout << "MID: " << result.mapper_id << std::endl;
        std::cout << "RID: " << result.reducer_id << std::endl;

        if (result.msg_type == 0){
            mapper(result.message, result.mapper_id, result.reducer_id);
        }
        else if (result.msg_type == 1){
            reducer(result.mapper_id, result.reducer_id);
        }
        else
        {
            //mr_msg goodbye = c->call("foo").as<mr_msg>(); //good bye, need return from rpc all the time
            cout << "worker finished" << endl;
            break;
        }
    }
}

int main(int argc, char *argv[]) {

    ofstream out("./log.txt");
    cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!
    
    // Creating a client that connects to the localhost on port 8080
    string ip("server.default");
    cout << "client started, ip = " << ip << endl;

    rpc::client client("server.default", 8080);
    worker(&client);
    return 0;
}