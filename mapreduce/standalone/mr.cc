#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <mutex>
#include <vector>
#include <string>
#include <queue>
#include <filesystem>
#include <algorithm>
#include <iterator>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string.hpp>
#include <cctype>
#include <regex>

using namespace std;
namespace fs = std::filesystem;

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

// system code
int mapper(string fn, int m_id, int n_reducer){

    ifstream ifs(fn, ios::in);
    string content((istreambuf_iterator<char>(ifs)), (istreambuf_iterator<char>()));
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
        ofstream out("./out/mr-" + to_string(m_id) + "-" + to_string(it - intermediate.begin()));
        out << output;
        out.close();
    };

    return 0;
}

int reducer(int n_mapper, int id) {

    vector<vector<string>> kvs;
    for (int i = 0; i < n_mapper; i++){
        string fname = string("./out/mr-") + to_string(i) + "-" + to_string(id);
        ifstream ifs(fname, ios::in);

        string content((istreambuf_iterator<char>(ifs)), (istreambuf_iterator<char>()));
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
    
    ofstream out("./out/mr-out-" + to_string(id));
    for (vector<vector<string>>::iterator it = kvs.begin(); it != kvs.end(); it++){
        
        vector<vector<string>>::iterator found = find_if(it + 1, kvs.end(), [&it](auto &cur){
            return (*it)[0].compare(cur[0]);
        });

        if (found == kvs.end())
            break;

        int count = found - it;
        vector<string> values(count, (*it)[1]);
        
        string result = Reduce((*it)[0], values);
        out << (*it)[0] << "," << result << endl;;
        it = found;
    }
    out.close();
    return 0;
}


// data in "/media/hmei/500G/6.824/src/main"
int main(int argc, char **argv){
    
    if (argc < 3){
        cout << "params needed. " << endl;
    }

    // strat mapers;
    vector<thread> mappers;
    int n_reducer = stoi(argv[2]);
    int map_idx = 0;
    for (const auto & entry : fs::directory_iterator(argv[1])){
        mappers.push_back(thread(mapper, string(entry.path().string()), map_idx++, n_reducer));
    }
    
    // all mappers done
    for_each(mappers.begin(), mappers.end(), [](auto &m){ m.join(); });

    // start reducers
    vector<thread> reducers;
    for (int i = 0; i < n_reducer; i++){
        reducers.push_back(thread(reducer, map_idx, i));
    }

    // all reducers done
    for_each(reducers.begin(), reducers.end(), [](auto &m){ m.join(); });

    cout << "Done!" << endl;
}