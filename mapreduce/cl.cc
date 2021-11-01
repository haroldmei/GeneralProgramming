#include <iostream>
#include <vector>

#include "rpc/client.h"
#include "./msg.hh"

using namespace std;

void worker(rpc::client *c){

    while(true){
        // Calling a function with paramters and converting the result to int
        mr_msg result = c->call("foo").as<mr_msg>();

        std::cout << "Type: " << result.msg_type << std::endl;
        std::cout << "Value: " << result.message << std::endl;

        if (result.msg_type == 0){
            mapper(result.message, result.mapper_id, result.reducer_id);
        }
        else if (result.msg_type == 1){
            reducer(result.mapper_id, result.reducer_id);
        }
        else
        {
            break;
        }
    }
}

int main() {

    // Creating a client that connects to the localhost on port 8080
    rpc::client client("127.0.0.1", 8080);

    // vector<thread> workers;
    // for (int i = 0; i < 3; i++){
    //     workers.push_back(thread(worker, &client));
    // }
    // for_each(workers.begin(), workers.end(), [](auto &i){i.join();});
    worker(&client);

    return 0;
}