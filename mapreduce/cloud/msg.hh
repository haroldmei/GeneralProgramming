#pragma once
#include "rpc/server.h"
#include <string>
#include <vector>

class mr_msg {
public:
    int msg_type;
    std::string message; //partition
    int mapper_id;
    int reducer_id;      //reducer id
    MSGPACK_DEFINE_ARRAY(msg_type, message, mapper_id, reducer_id);
};

using namespace std;
vector<vector<string>> Map(string content);
string Reduce(string key, vector<string> values);
int mapper(string fn, int m_id, int n_reducer);
int reducer(int n_mapper, int id);
string const bucket_name;