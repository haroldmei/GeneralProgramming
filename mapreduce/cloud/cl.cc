#include <iostream>
#include <fstream>
#include <vector>
#include <boost/asio.hpp>

#include "rpc/client.h"
#include "./msg.hh"
#include "google/cloud/storage/client.h"

using namespace std;
namespace gcs = ::google::cloud::storage;
namespace ip = boost::asio::ip;

void worker(rpc::client *c, gcs::ObjectWriteStream &out){

    while(true){
        // Calling a function with paramters and converting the result to int
        out << "worker started" << endl;
        mr_msg result = c->call("foo").as<mr_msg>();

        out << "Type: " << result.msg_type << std::endl;
        out << "Value: " << result.message << std::endl;
        out << "MID: " << result.mapper_id << std::endl;
        out << "RID: " << result.reducer_id << std::endl;

        if (result.msg_type == 0){
            mapper(result.message, result.mapper_id, result.reducer_id);
        }
        else if (result.msg_type == 1){
            reducer(result.mapper_id, result.reducer_id);
        }
        else
        {
            //mr_msg goodbye = c->call("foo").as<mr_msg>(); //good bye, need return from rpc all the time
            out << "worker finished" << endl;
            break;
        }
    }
}

int main(int argc, char *argv[]) {

    // Creating a client that connects to the localhost on port 8080
    rpc::client c("server.mapreduce", 8080);
    
    extern gcs::Client client;
    string out("log-client-" + ip::host_name());
    auto writer = client.WriteObject(bucket_name, out);

    string ip("server.mapreduce");
    writer << "client started, ip = " << ip << endl;

    worker(&c, writer);
    writer.Close();
    return 0;
}