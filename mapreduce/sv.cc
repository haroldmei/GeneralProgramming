#include <iostream>
#include <mutex>
#include <filesystem>

#include "rpc/server.h"

#include "./msg.hh"


using namespace std;
namespace fs = filesystem;

vector<string> partitions;
int num_partition = 0;
int n_reducer;
mutex locker;

mr_msg foo() {

    std::cout << "foo was called!" << std::endl;

    locker.lock();
    mr_msg msg;
    if (partitions.size()){
        string to_map = partitions.back();
        partitions.pop_back();
        msg = {0, to_map, partitions.size(), n_reducer};
    }
    else if (n_reducer){
        msg = {1, string(""), num_partition, --n_reducer};
    }
    else{
        msg = {2, string(""), num_partition, 0};
    }

    locker.unlock();
    if (msg.msg_type == 2){
        exit(0);
    }
    return msg;
}

int main(int argc, char *argv[]) {

    if (argc < 3){
        cout << "params needed. " << endl;
    }

    n_reducer = stoi(argv[2]);
    for (const auto & entry : fs::directory_iterator(argv[1])){
        partitions.push_back(string(entry.path().string()));
    }
    num_partition = partitions.size();

    // Creating a server that listens on port 8080
    rpc::server srv(8080);
    srv.bind("foo", &foo);

    // Run the server loop.
    srv.run();

    return 0;
}