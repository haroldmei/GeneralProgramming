/*Given two words (beginWord and endWord), and a dictionary's word list, find the length of shortest transformation sequence from beginWord to endWord, such that:

Only one letter can be changed at a time.
Each transformed word must exist in the word list. Note that beginWord is not a transformed word.
For example,

Given:
beginWord = "hit"
endWord = "cog"
wordList = ["hot","dot","dog","lot","log","cog"]
As one shortest transformation is "hit" -> "hot" -> "dot" -> "dog" -> "cog",
return its length 5.

Note:
Return 0 if there is no such transformation sequence.
All words have the same length.
All words contain only lowercase alphabetic characters.
You may assume no duplicates in the word list.
You may assume beginWord and endWord are non-empty and are not the same.
UPDATE (2017/1/20):
The wordList parameter had been changed to a list of strings (instead of a set of strings). Please reload the code definition to get the latest changes.
*/

class Solution {
public:

    int ladderLength(string beginWord, string endWord, unordered_set<string>& wordList) {

        wordList.insert(endWord);
        unordered_map<string, string> m;
        vector<string> froms = { beginWord };
        int i = 0;
        while (i < froms.size()){
            string b = froms[i];
            for (char &c : b){
                char cur = c;
                for (c = 'a'; c <= 'z'; ++c){
                    if (c != cur && wordList.count(b)){
                        m[b] = froms[i];
                        froms.push_back(b);
                        wordList.erase(b);
                    }
                }
                c = cur;
            }
            i++;
        }

        if (!m.size())
            return 0;

        int res = 0;
        while (endWord != beginWord && m.count(endWord)){
            endWord = m[endWord];
            res++;
        }
        return res + 1;
    }
};