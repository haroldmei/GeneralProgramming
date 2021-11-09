#include <iostream>
#include <fstream>
#include <mutex>
#include <filesystem>

#include "rpc/server.h"
#include "google/cloud/storage/client.h"

#include "./msg.hh"


using namespace std;
namespace fs = filesystem;

vector<string> partitions;
int num_partition = 0;
int n_reducer;
mutex locker;
int wip = 0;

mr_msg foo() {

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
    else if (wip == 0){
        msg = {2, string(""), num_partition, 0};
        wip = 1;
    }
    else {
        wip = 2;
    }

    std::cout << "foo was called!" << wip << " n_reducer: " << n_reducer << ", n_mapper: " << partitions.size() << std::endl;

    locker.unlock();
    if (wip == 2){
        wip = 0;
        //exit(0);
    }
    return msg;
}

int main(int argc, char *argv[]) {

    if (argc < 3){
        cout << "params needed. " << endl;
    }

    ofstream out("./log.txt");
    cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!

    n_reducer = stoi(argv[2]);
    for (const auto & entry : fs::directory_iterator(argv[1])){
        partitions.push_back(string(entry.path().string()));
    }

    namespace gcs = ::google::cloud::storage;
    auto client = gcs::Client();
    auto f = [](gcs::Client client, std::string const& bucket_name) {
      for (auto&& object_metadata : client.ListObjects(bucket_name)) {
        if (!object_metadata) {
          throw std::runtime_error(object_metadata.status().message());
        }

        std::cout << "bucket_name=" << object_metadata->bucket()
                  << ", object_name=" << object_metadata->name() << "\n";
      }
    };
    f(client, "gs://gcp-wow-rwds-ai-mlchapter-dev-bucket/data");
    num_partition = partitions.size();

    // Creating a server that listens on port 8080
    rpc::server srv(8080);
    srv.bind("foo", &foo);

    // Run the server loop.
    srv.run();

    return 0;
}