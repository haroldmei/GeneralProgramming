#include <algorithm>
#include <fstream>
#include <vector>
#include <string>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>
#include "google/cloud/storage/client.h"
#include "msg.hh"

using namespace std;
namespace gcs = ::google::cloud::storage;
string const bucket_name = "gcp-wow-rwds-ai-mlchapter-dev-bucket";
gcs::Client client = gcs::Client();

// cliend code
vector<vector<string>> Map(string content){

    vector<string> words;
    boost::split(words, content, boost::is_any_of(" ,.:\n\t\r"));

    vector<vector<string>> kvs;
    for(string &word: words){
        if (!word.length())
            continue;
        vector<string> kv({word, "1"});
        kvs.push_back(kv);
    }
    return kvs;
}

string Reduce(string key, vector<string> values){
    return to_string(values.size());
}

string ReadObject(string bucket, string object){
    auto reader = client.ReadObject(bucket_name, object);
    if (!reader) {
        std::cerr << "Error reading object: " << reader.status() << "\n";
        return "";
    }

    std::string contents{std::istreambuf_iterator<char>{reader}, {}};
    return contents;
}

void WriteObject(string bucket, string object, string content){

    cout << bucket << endl;
    cout << object << endl;

    auto writer = client.WriteObject(bucket_name, object);
    writer << content;
    writer.Close();

    if (writer.metadata()) {
        std::cout << "Successfully created object: " << *writer.metadata() << "\n";
    } else {
        std::cerr << "Error creating object: " << writer.metadata().status() << "\n";
    }
}
// system code
int mapper(string fn, int m_id, int n_reducer){

    //ifstream ifs(fn, ios::in);
    //string content((istreambuf_iterator<char>(ifs)), (istreambuf_iterator<char>()));
    string content = ReadObject(bucket_name, fn);

    auto kvs = Map(content);

    //initialize n_reducer number of buckets
    vector<vector<vector<string>>> intermediate(n_reducer, vector<vector<string>>());
    for_each(kvs.begin(), kvs.end(), [&intermediate, n_reducer](auto &kv){
        int index = hash<string>{}(kv[0]) % n_reducer;
        intermediate[index].push_back(kv);
    });
    for (auto it = intermediate.begin(); it != intermediate.end(); it++){
        string output;
        for_each(it->begin(), it->end(), [&output](auto &e){
            output += e[0];
            output += ",";
            output += e[1];
            output += "\n";
        });

        //ofstream out("./out/mr-" + to_string(m_id) + "-" + to_string(it - intermediate.begin()));
        //out << output;
        //out.close();
        string out("out/mr-" + to_string(m_id) + "-" + to_string(it - intermediate.begin()));
        WriteObject(bucket_name, out, output);
    };

    return 0;
}

int reducer(int n_mapper, int id) {

    vector<vector<string>> kvs;
    for (int i = 0; i < n_mapper; i++){
        string fname = string("./out/mr-") + to_string(i) + "-" + to_string(id);

        //ifstream ifs(fname, ios::in);
        //string content((istreambuf_iterator<char>(ifs)), (istreambuf_iterator<char>()));
        string content = ReadObject(bucket_name, fname);

        vector<string> lines;
        boost::split(lines, content, boost::is_any_of("\n"));

        for_each(lines.begin(), lines.end(), [&kvs](auto &it){
            vector<string> kv;
            boost::split(kv, it, boost::is_any_of(","));
            if (kv.size() < 2){
                //cout << it << endl;
                return;
            }
            //cout << kv.size() << endl;
            kvs.push_back(vector<string>({kv[0], kv[1]}));
        });
    }
    sort(kvs.begin(), kvs.end());
    
    string out("./out/mr-out-" + to_string(id));
    for (vector<vector<string>>::iterator it = kvs.begin(); it != kvs.end(); it++){
        
        vector<vector<string>>::iterator found = find_if(it + 1, kvs.end(), [&it](auto &cur){
            return (*it)[0].compare(cur[0]);
        });

        if (found == kvs.end())
            break;

        int count = found - it;
        vector<string> values(count + 1, (*it)[1]);
        
        string result = Reduce((*it)[0], values);

        WriteObject(bucket_name, out, result);
        it = found;
    }

    return 0;
}
